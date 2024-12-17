from pydantic import BaseModel, Field
from enum import Enum

class Staff(BaseModel):
    accountID: int
    admin: bool = False

class Feature(Enum):
    RATE = 0
    FEATURE = 1
    EPIC = 2
    LEGENDARY = 3
    MYTHIC = 4

class suggestGJStars20(BaseModel):
    gameVersion: int | None = None
    binaryVersion: int | None = None
    udid: str | None = None
    uuid: str | None = None
    accountID: int | None = None
    gjp2: str | None = None
    levelID: int | None = None
    stars: int | None = None
    feature: Feature | None = None
    gdw: bool = False
    secret: str | None = None

class Reassign(BaseModel):
    promote: bool
    accountID: int
    gjp2: str
    ownerID: int
