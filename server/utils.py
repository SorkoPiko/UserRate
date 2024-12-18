import requests
import threading
import queue
import time
import asyncio

class AccountChecker:
    def __init__(self):
        self.q = queue.Queue()
        self.lock = threading.Lock()
        self.loop = asyncio.get_event_loop()
        self.thread = threading.Thread(target=self.worker)
        self.thread.daemon = True
        self.thread.start()

    def worker(self):
        while True:
            accountid, gjp, callback, *args = self.q.get()
            with self.lock:
                result = self.check_account(accountid, gjp)
                self.loop.call_soon_threadsafe(asyncio.create_task, callback(result, *args))
                time.sleep(1)
            self.q.task_done()

    @staticmethod
    def check_account(accountid, gjp) -> bool:
        # thanks GDAuth
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
        self.q.put((accountid, gjp, callback, *args))