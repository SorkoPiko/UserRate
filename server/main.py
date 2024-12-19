from fastapi import FastAPI, HTTPException, Request
from fastapi.responses import HTMLResponse
from dotenv import load_dotenv
from os import environ
import hashlib, asyncio

from db import UserRateDB
from models import suggestGJStars20, Sort, Reassign
from utils import AccountChecker

load_dotenv()
db = UserRateDB(f"mongodb+srv://{environ.get('MONGO_USERNAME')}:{environ.get('MONGO_PASSWORD')}@{environ.get('MONGO_ENDPOINT')}")
checker = AccountChecker()

app = FastAPI(
    title="User Rate API",
    description="API for the Geode mod User Rate",
    version="1.0.0",
    docs_url="/"
)

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
@app.post("/suggest", response_class=HTMLResponse)
async def suggest(request: Request):
    raw_form = await request.form()
    form_dict = {key: value for key, value in raw_form.items()}

    form_dict = convert_fields_to_int(form_dict, ["gameVersion", "binaryVersion", "accountID", "levelID", "stars", "feature"])
    form_dict = convert_fields_to_bool(form_dict, ["gdw"])

    form = suggestGJStars20(**form_dict)

    if not form.gjp2:
        return "-2"

    if form.stars is None or form.feature is None:
        return "-2"

    if form.stars < 0 or form.stars > 10:
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

@app.get("/mod/sent")
async def get_sent_levels(sort: Sort = Sort.TOP, page: int = 0, accountID: int = 0, gjp2: str = ""):
    if accountID == 0 or not gjp2:
        raise HTTPException(status_code=401, detail="You must provide authentication to view sent levels")

    staff = db.find_staff_by_id(accountID)
    if staff is None:
        raise HTTPException(status_code=401, detail="You must be a moderator to view sent levels")

    future = asyncio.Future()

    async def callback(result: bool):
        future.set_result(result)

    checker.queue_check(accountID, gjp2, callback)

    futureResult = await future

    if not futureResult:
        raise HTTPException(status_code=400, detail="Invalid credentials")

    return db.get_sent_levels(sort, page)

@app.post("/admin/reassign")
async def reassign_moderator(data: Reassign):
    if data.auth.accountID == 0 or not data.auth.gjp2:
        raise HTTPException(status_code=401, detail="You must provide authentication to view sent levels")

    staff = db.find_staff_by_id(data.auth.accountID)
    if staff is None or not staff.admin:
        raise HTTPException(status_code=401, detail="You must be an admin to reassign moderators")

    future = asyncio.Future()

    async def callback(result: bool):
        future.set_result(result)

    checker.queue_check(data.auth.accountID, data.auth.gjp2, callback)

    futureResult = await future

    if not futureResult:
        raise HTTPException(status_code=400, detail="Invalid credentials")

    if data.promote:
        db.promote_mod(data.accountID)
    else:
        db.demote_mod(data.accountID)

    return {"message": "Success"}

@app.get("/mods")
async def get_mods():
    staff = db.get_all_staff()
    return {"mods": [mod.accountID for mod in staff if not mod.admin], "admins": [admin.accountID for admin in staff if admin.admin]}
