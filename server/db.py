from pymongo.mongo_client import MongoClient
from pymongo.server_api import ServerApi
from pymongo.collection import Collection
from pymongo.database import Database
from pymongo.errors import DuplicateKeyError

from models import Staff

class UserRateDB:
    def __init__(self, connection_string: str):
        self.client = MongoClient(connection_string, server_api=ServerApi('1'))

    def get_database(self, db_name: str) -> Database:
        return self.client[db_name]

    def get_collection(self, db_name: str, collection_name: str) -> Collection:
        db = self.get_database(db_name)
        return db[collection_name]

    def find_staff_by_id(self, staff_id: int) -> Staff:
        collection = self.get_collection("data", "staff")
        raw = collection.find_one({"_id": staff_id})
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