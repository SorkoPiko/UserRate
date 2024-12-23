from pydantic import BaseModel
from enum import Enum, IntEnum

class Staff(BaseModel):
    accountID: int
    admin: bool = False

class Feature(Enum):
    RATE = 0
    FEATURE = 1
    EPIC = 2
    LEGENDARY = 3
    MYTHIC = 4

class Demon(Enum):
    EASY = 1
    MEDIUM = 2
    HARD = 3
    INSANE = 4
    EXTREME = 5

class Sort(IntEnum):
    TOP = 0
    RECENT = 1
    OLDEST = 2

class Auth(BaseModel):
    accountID: int
    gjp2: str

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

class rateGJDemon21(BaseModel):
    gameVersion: int | None = None
    binaryVersion: int | None = None
    udid: str | None = None
    uuid: str | None = None
    accountID: int | None = None
    gjp2: str | None = None
    levelID: int | None = None
    rating: Demon | None = None
    gdw: bool = False
    secret: str | None = None

class Sent(BaseModel):
    sort: Sort = Sort.TOP
    page: int = 0
    accountID: int
    gjp2: str

class Reassign(BaseModel):
    promote: bool
    accountID: int

class Clear(BaseModel):
    levelID: int

class Rate(BaseModel):
    levelID: int
    stars: int
    feature: Feature
