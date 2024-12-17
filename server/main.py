from fastapi import FastAPI, Request
from fastapi.responses import HTMLResponse
from dotenv import load_dotenv
from os import environ
import hashlib

from db import UserRateDB
from models import suggestGJStars20, Reassign
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
@app.post("/mod/suggest", response_class=HTMLResponse)
async def suggest(request: Request):
    raw_form = await request.form()
    form_dict = {key: value for key, value in raw_form.items()}

    form_dict = convert_fields_to_int(form_dict, ["gameVersion", "binaryVersion", "accountID", "levelID", "stars", "feature"])
    form_dict = convert_fields_to_bool(form_dict, ["gdw"])
    form_dict = censor(form_dict)

    form = suggestGJStars20(**form_dict)

    if not form.gjp2: return "-2"

    print(form)

    return "1"

@app.post("/demon", response_class=HTMLResponse)
async def demon(request: Request):
    raw_form = await request.form()
    form_dict: dict = {key: value for key, value in raw_form.items()}
    print("Demon")
    print(form_dict)
    return {"message": "Demon"}

async def reassign(result: bool, accountID: int, promote: bool):
    if not result:
        return "-1"
    if promote:
        await db.promote_mod(accountID)
    else:
        await db.demote_mod(accountID)
    return "1"

@app.post("/reassign")
async def reassign_check(data: Reassign):
    checker.queue_check(data.ownerID, data.gjp2, reassign, data.accountID, data.promote)
