import requests
import multiprocessing
from os import getpid
import sys

def make_request():
    response = requests.get(web_url)
    print(getpid(), response.headers)


if len(sys.argv) != 3:
    print("Usage: testRequest.py [amount of processes] [port]")
    sys.exit()


web_url = "http://localhost:" + sys.argv[2]
n = int(sys.argv[1])
pArr = []

for i in range(n):
    pArr.append(multiprocessing.Process(target=make_request))
    pArr[i].start()

for i in range(n):
    pArr[i].join()

print("All done.")


