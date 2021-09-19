import requests

n = 5
web_url = "http://localhost:9090"

for i in range(n):
    response = requests.get(web_url)
    print(response.headers)
