from fastapi import FastAPI, HTTPException, Request, Depends, Query
from fastapi.responses import HTMLResponse
from fastapi.security import OAuth2PasswordBearer
from dotenv import load_dotenv
from os import environ
import hashlib, asyncio, jwt
from datetime import datetime, timedelta, UTC
from discord_webhook import DiscordWebhook, DiscordEmbed

from db import UserRateDB
from models import suggestGJStars20, Sort, Reassign, Auth, Clear, Rate
from utils import AccountChecker

load_dotenv()
db = UserRateDB(f"mongodb+srv://{environ.get('MONGO_USERNAME')}:{environ.get('MONGO_PASSWORD')}@{environ.get('MONGO_ENDPOINT')}")
checker = AccountChecker()
oauth2_scheme = OAuth2PasswordBearer(tokenUrl="token")

SECRET_KEY = environ.get("SECRET_KEY")

app = FastAPI(
    title="User Rate API",
    description="API for the Geode mod User Rate",
    version="1.0.0",
    docs_url="/"
)

def create_access_token(data: dict, expires_delta: timedelta = timedelta(hours=6)):
    to_encode = data.copy()
    expire = datetime.now(UTC) + expires_delta
    to_encode.update({"exp": expire})
    encoded_jwt = jwt.encode(to_encode, SECRET_KEY, algorithm="HS256")
    return encoded_jwt

def verify_token(token: str = Depends(oauth2_scheme)):
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=["HS256"])
        account_id: int = payload.get("accountID")
        role: str = payload.get("role")
        if account_id is None or role is None:
            raise HTTPException(status_code=401, detail="Invalid token")

        staff = db.find_staff_by_id(account_id)
        if staff is None or (role == "admin" and not staff.admin) or (role == "moderator" and staff.admin):
            raise HTTPException(status_code=401, detail="Invalid token")

        return {"accountID": account_id, "role": role}
    except jwt.PyJWTError:
        raise HTTPException(status_code=401, detail="Invalid token")

def censor(form_dict: dict) -> dict:
    if "gjp2" in form_dict:
        form_dict["gjp2"] = hashlib.sha256(form_dict["gjp2"].encode()).hexdigest()
    return form_dict

def convert_fields_to_int(form_dict: dict, fields: list) -> dict:
    for field in fields:
        if field in form_dict:
            form_dict[field] = int(form_dict[field])
    return form_dict

def convert_fields_to_bool(form_dict: dict, fields: list) -> dict:
    for field in fields:
        if field in form_dict:
            form_dict[field] = bool(int(form_dict[field]))
    return form_dict

@app.post("/token")
async def login(auth: Auth):
    if not auth.gjp2 or not auth.accountID:
        raise HTTPException(status_code=400, detail="Invalid credentials")

    staff = db.find_staff_by_id(auth.accountID)
    if staff is None:
        raise HTTPException(status_code=400, detail="Invalid credentials")

    future = asyncio.Future()

    async def callback(result: bool):
        future.set_result(result)

    checker.queue_check(auth.accountID, auth.gjp2, callback)

    futureResult = await future

    if not futureResult:
        raise HTTPException(status_code=400, detail="Invalid credentials")

    role = "admin" if staff.admin else "moderator"
    access_token = create_access_token(data={"accountID": auth.accountID, "role": role})
    return {"access_token": access_token, "token_type": "bearer"}

@app.post("/suggest", response_class=HTMLResponse)
async def suggest(request: Request):
    raw_form = await request.form()
    form_dict = {key: value for key, value in raw_form.items()}

    form_dict = convert_fields_to_int(form_dict, ["gameVersion", "binaryVersion", "accountID", "levelID", "stars", "feature"])
    form_dict = convert_fields_to_bool(form_dict, ["gdw"])

    if "feature" in form_dict and form_dict["feature"] not in [0, 1, 2, 3, 4]:
        return "-2"

    form = suggestGJStars20(**form_dict)

    if not form.gjp2:
        return "-2"

    if form.stars is None or form.feature is None:
        return "-2"

    if form.stars < 1 or form.stars > 10:
        return "-2"

    sendDict = {
        "ip": hashlib.sha256(request.client.host.encode()).hexdigest(),
        "levelID": form.levelID,
        "stars": form.stars,
        "feature": form.feature.value
    }

    db.send(sendDict)
    print(f"{request.client.host} suggested {form.stars} stars with a {form.feature} rating for level {form.levelID}")

    return "1"

@app.post("/demon", response_class=HTMLResponse)
async def demon(request: Request):
    raw_form = await request.form()
    form_dict: dict = {key: value for key, value in raw_form.items()}
    return {"message": "Demon"}

@app.get("/rated")
async def check_rated_levels(levelIDs: str):
    try:
        id_list = [int(id) for id in levelIDs.split(',')]
    except Exception:
        raise HTTPException(status_code=422, detail="Invalid level IDs")
    return db.check_rated_levels(id_list)

@app.get("/mod/sent")
async def get_sent_levels(
        sort: Sort = Sort.TOP,
        page: int = 0,
        min_send_count: int = 0,
        max_send_count: int = None,
        min_avg_stars: int = 1,
        max_avg_stars: int = 10,
        min_avg_feature: int = 0,
        max_avg_feature: int = 4,
        token_data: dict = Depends(verify_token)
):
    return db.get_sent_levels(sort, page, min_send_count, max_send_count, min_avg_stars, max_avg_stars, min_avg_feature, max_avg_feature)

@app.post("/mod/rate")
async def rate_level(data: Rate, token_data: dict = Depends(verify_token)):
    if data.stars < 1 or data.stars > 10:
        raise HTTPException(status_code=400, detail="Stars must be between 1 and 10")

    db.rate(data.levelID, data.stars, data.feature.value)

    webhook = DiscordWebhook(url=environ.get("WEBHOOK_URL"), rate_limit_retry=True)
    embed = DiscordEmbed(
        title="Level Rated",
        description=f"Level ID: {data.levelID}\nStars: {data.stars}\nFeature: {data.feature}",
        color=0x00C04B
    )
    webhook.add_embed(embed)

    webhook.execute()

    return {"message": "Success"}

@app.post("/admin/reassign")
async def reassign_moderator(data: Reassign, token_data: dict = Depends(verify_token)):
    if token_data["role"] != "admin":
        raise HTTPException(status_code=400, detail="You must be an admin to reassign moderators")

    if data.promote:
        db.promote_mod(data.accountID)
    else:
        db.demote_mod(data.accountID)

    return {"message": "Success"}

@app.post("/admin/clear")
async def clear_sends(data: Clear, token_data: dict = Depends(verify_token)):
    if token_data["role"] != "admin":
        raise HTTPException(status_code=400, detail="You must be an admin to clear sends")

    db.clear_sends_for_level(data.levelID)
    return {"message": "Success"}

@app.get("/mods")
async def get_mods():
    staff = db.get_all_staff()
    return {"mods": [mod.accountID for mod in staff if not mod.admin], "admins": [admin.accountID for admin in staff if admin.admin]}
