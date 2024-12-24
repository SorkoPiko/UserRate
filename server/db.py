from pymongo.mongo_client import MongoClient
from pymongo.server_api import ServerApi
from pymongo.collection import Collection
from pymongo.database import Database
from pymongo.errors import DuplicateKeyError
from pymongo import DESCENDING, ASCENDING
from datetime import datetime, UTC

from models import Staff, Sort, Demon


class UserRateDB:
    def __init__(self, connection_string: str):
        self.client = MongoClient(connection_string, server_api=ServerApi('1'))
        self.create_indexes()

    def create_indexes(self):
        sends = self.get_collection("data", "sends")
        sends.create_index([("ip", ASCENDING), ("levelID", ASCENDING)], unique=True)

        demon = self.get_collection("data", "demon")
        demon.create_index([("ip", ASCENDING), ("levelID", ASCENDING)], unique=True)


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

    def get_level_spreads(self, level_id: int) -> dict:
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

    def get_sent_levels(
            self,
            sort: Sort,
            page: int = 0,
            min_send_count: int = 0,
            max_send_count: int = None,
            min_avg_stars: int = 0,
            max_avg_stars: int = 10,
            min_avg_feature: int = 0,
            max_avg_feature: int = 4,
            page_size: int = 50
    ) -> dict:
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
                "mostRecentSend": {"$max": "$timestamp"},
                "avgStars": {"$avg": "$stars"},
                "avgFeature": {"$avg": "$feature"}
            }},
            {"$match": {
                "sendCount": {"$gte": min_send_count},
                "avgStars": {"$gte": min_avg_stars},
                "avgFeature": {"$gte": min_avg_feature}
            }},
            {"$sort": dict(sort_criteria)},
            {"$skip": page * page_size},
            {"$limit": page_size}
        ]

        if max_send_count is not None:
            pipeline[1]["$match"]["sendCount"]["$lte"] = max_send_count
        if max_avg_stars is not None:
            pipeline[1]["$match"]["avgStars"]["$lte"] = max_avg_stars
        if max_avg_feature is not None:
            pipeline[1]["$match"]["avgFeature"]["$lte"] = max_avg_feature

        results = list(collection.aggregate(pipeline))

        for result in results:
            result["levelID"] = result.pop("_id")
            result.update(self.get_level_spreads(result["levelID"]))
            result.pop("sendCount")
            result.pop("mostRecentSend")
            result.pop("avgStars")
            result.pop("avgFeature")

        return {
            "levels": results
        }

    def clear_sends_for_level(self, level_id: int):
        collection = self.get_collection("data", "sends")
        collection.delete_many({"levelID": level_id})

    def rate(self, level_id: int, stars: int, feature: int, account_id: int):
        rates_collection = self.get_collection("data", "rates")

        # Insert into the rates collection
        rates_collection.insert_one({
            "_id": level_id,
            "stars": stars,
            "feature": feature,
            "ratedBy": account_id,
            "timestamp": datetime.now(UTC)
        })

        # Clear sends for the specified levelID
        self.clear_sends_for_level(level_id)

    def derate(self, level_id: int):
        rates_collection = self.get_collection("data", "rates")
        rates_collection.delete_one({"_id": level_id})
        self.clear_demon_for_level(level_id)

    def rated_level(self, level_id: int) -> dict:
        rates_collection = self.get_collection("data", "rates")
        rating: dict = rates_collection.find_one({"_id": level_id}, {"_id": 1, "stars": 1, "feature": 1})
        if rating is not None: rating["levelID"] = rating.pop("_id")
        return rating

    def check_rated_levels(self, level_ids: list[int]) -> dict:
        rates_collection = self.get_collection("data", "rates")
        ratings = rates_collection.find({"_id": {"$in": level_ids}}, {"_id": 1, "stars": 1, "feature": 1})

        rated_levels = []
        demon_level_ids = []

        for rating in ratings:
            rating["levelID"] = rating.pop("_id")
            if rating["stars"] == 10:
                demon_level_ids.append(rating["levelID"])
            rated_levels.append(rating)

        if demon_level_ids:
            demon_difficulties = self.check_demon_difficulty(demon_level_ids)
            demon_difficulty_map = {demon["levelID"]: demon["avgRating"] for demon in demon_difficulties["levels"]}

            for level in rated_levels:
                if level["levelID"] in demon_difficulty_map:
                    level["avgDemonRating"] = demon_difficulty_map[level["levelID"]]

        return {"levels": rated_levels}

    def demon(self, data: dict):
        collection = self.get_collection("data", "demon")
        data["timestamp"] = datetime.now(UTC)
        collection.update_one(
            {"ip": data["ip"], "levelID": data["levelID"]},
            {"$set": data},
            upsert=True
        )

    def clear_demon_for_level(self, level_id: int):
        collection = self.get_collection("data", "demon")
        collection.delete_many({"levelID": level_id})

    def check_demon_difficulty(self, level_ids: list[int]) -> dict:
        demon_collection = self.get_collection("data", "demon")

        pipeline = [
            {"$match": {"levelID": {"$in": level_ids}}},
            {"$group": {
                "_id": "$levelID",
                "avgRating": {"$avg": "$rating"}
            }}
        ]

        results = list(demon_collection.aggregate(pipeline))

        for result in results:
            result["levelID"] = result.pop("_id")

        return {"levels": results}

    def get_latest_rated_levels(self, max_levels: int = 200) -> list[dict]:
        rates_collection = self.get_collection("data", "rates")
        latest_rated_levels = rates_collection.find().sort("timestamp", DESCENDING).limit(max_levels)

        rated_levels = []
        for level in latest_rated_levels:
            rated_levels.append(level["_id"])

        return rated_levels