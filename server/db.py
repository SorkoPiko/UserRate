from pymongo.mongo_client import MongoClient
from pymongo.server_api import ServerApi
from pymongo.collection import Collection
from pymongo.database import Database
from pymongo.errors import DuplicateKeyError
from pymongo import DESCENDING, ASCENDING
from datetime import datetime, UTC

from models import Staff, Sort

class UserRateDB:
    def __init__(self, connection_string: str):
        self.client = MongoClient(connection_string, server_api=ServerApi('1'))
        self.create_indexes()

    def create_indexes(self):
        collection = self.get_collection("data", "sends")
        collection.create_index([("ip", ASCENDING), ("levelID", ASCENDING)], unique=True)


    def get_database(self, db_name: str) -> Database:
        return self.client[db_name]

    def get_collection(self, db_name: str, collection_name: str) -> Collection:
        db = self.get_database(db_name)
        return db[collection_name]

    def get_all_staff(self) -> list[Staff]:
        collection = self.get_collection("data", "staff")
        raw_staff = collection.find()
        return [Staff(accountID=staff["_id"], admin=staff.get("admin", False)) for staff in raw_staff]

    def find_staff_by_id(self, staff_id: int) -> Staff | None:
        collection = self.get_collection("data", "staff")
        raw = collection.find_one({"_id": staff_id})
        if raw is None:
            return None
        if "admin" not in raw:
            return Staff(accountID=raw["_id"])
        return Staff(accountID=raw["_id"], admin=raw["admin"])

    def demote_mod(self, account_id: int):
        collection = self.get_collection("data", "staff")
        if collection.find_one({"_id": account_id, "admin": True}):
            return
        collection.delete_one({"_id": account_id})

    def promote_mod(self, account_id: int):
        collection = self.get_collection("data", "staff")
        try:
            collection.insert_one({"_id": account_id})
        except DuplicateKeyError:
            pass

    def send(self, data: dict):
        collection = self.get_collection("data", "sends")
        data["timestamp"] = datetime.now(UTC)
        collection.update_one(
            {"ip": data["ip"], "levelID": data["levelID"]},
            {"$set": data},
            upsert=True
        )

    def get_level_spreads(self, level_id: int):
        collection = self.get_collection("data", "sends")

        pipeline = [
            {"$match": {"levelID": level_id}},
            {"$group": {
                "_id": "$levelID",
                "feature_spread": {"$push": "$feature"},
                "stars_spread": {"$push": "$stars"}
            }}
        ]

        result = collection.aggregate(pipeline).next()

        feature_spread = {
            0: 0,
            1: 0,
            2: 0,
            3: 0,
            4: 0
        }
        stars_spread = {
            1: 0,
            2: 0,
            3: 0,
            4: 0,
            5: 0,
            6: 0,
            7: 0,
            8: 0,
            9: 0,
            10: 0
        }

        for feature in result["feature_spread"]:
            feature_spread[feature] += 1

        for stars in result["stars_spread"]:
            stars_spread[stars] += 1

        return {
            "feature_spread": feature_spread,
            "stars_spread": stars_spread
        }

    def get_sent_levels(self, sort: Sort, page: int = 0, page_size: int = 50):
        collection = self.get_collection("data", "sends")

        if sort == Sort.RECENT:
            sort_criteria = [("timestamp", DESCENDING)]
        elif sort == Sort.TOP:
            sort_criteria = [("sendCount", DESCENDING)]
        elif sort == Sort.OLDEST:
            sort_criteria = [("timestamp", ASCENDING)]
        else:
            raise ValueError("Invalid sort criteria")

        pipeline = [
            {"$group": {
                "_id": "$levelID",
                "sendCount": {"$sum": 1},
                "mostRecentSend": {"$max": "$timestamp"}
            }},
            {"$sort": dict(sort_criteria)},
            {"$skip": page * page_size},
            {"$limit": page_size}
        ]

        results = list(collection.aggregate(pipeline))

        for result in results:
            result["levelID"] = result.pop("_id")
            result.update(self.get_level_spreads(result["levelID"]))
            result.pop("sendCount")
            result.pop("mostRecentSend")

        return {
            "levels": results
        }