import requests
import threading
import queue
import time
import asyncio
from typing import Optional

from models import Feature

class AccountChecker:
    def __init__(self):
        self.q: queue.Queue = queue.Queue()
        self.lock = threading.Lock()
        self.loop: Optional[asyncio.AbstractEventLoop] = None
        self.thread: Optional[threading.Thread] = None
        self.running = threading.Event()

    def start(self, loop: asyncio.AbstractEventLoop):
        """Start the worker thread with the given event loop"""
        self.loop = loop
        self.running.set()
        self.thread = threading.Thread(target=self.worker)
        self.thread.daemon = True
        self.thread.start()

    def stop(self):
        """Stop the worker thread"""
        if self.running.is_set():
            self.running.clear()
            self.q.put((None, None, None))
            if self.thread:
                self.thread.join(timeout=5)

    def worker(self):
        while self.running.is_set():
            try:
                accountid, gjp, callback, *args = self.q.get(timeout=1)
                if accountid is None:
                    break

                with self.lock:
                    if not self.running.is_set():
                        break

                    result = self.check_account(accountid, gjp)
                    if self.running.is_set() and self.loop and not self.loop.is_closed():
                        self.loop.call_soon_threadsafe(
                            lambda: asyncio.create_task(callback(result, *args))
                        )
                    time.sleep(1)
                self.q.task_done()
            except queue.Empty:
                continue
            except Exception as e:
                print(f"Error in worker thread: {e}")

    @staticmethod
    def check_account(accountid, gjp) -> bool:
        data = {
            "targetAccountID": accountid,
            "accountID": accountid,
            "gjp2": gjp,
            "secret": "Wmfd2893gb7"
        }

        headers = {
            "User-Agent": ""
        }

        req = requests.post('http://www.boomlings.com/database/getGJUserInfo20.php', data=data, headers=headers)

        if req.text == "-1": return False

        split = req.text.split(":")
        if split[0] != "1": return False

        return True

    def queue_check(self, accountid, gjp, callback, *args):
        if self.running.is_set():
            self.q.put((accountid, gjp, callback, *args))

def featureToString(feature: Feature) -> str:
    return {
        Feature.RATE: "Rate",
        Feature.FEATURE: "Feature",
        Feature.EPIC: "Epic",
        Feature.LEGENDARY: "Legendary",
        Feature.MYTHIC: "Mythic"
    }[feature]