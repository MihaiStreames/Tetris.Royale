

import requests
from request_objects import Request, Response


class DatabaseServiceClient:

    def __init__(self, db_ip: str = "127.0.0.1", db_port: int = 5000, db_route: str = '/db/requests') -> None:
        self.db_ip: str = db_ip
        self.db_port: int = db_port
        self.db_route: str = db_route

    def send_request(self, request: Request) -> Response:

        server_http: str = f"http://{self.db_ip}:{self.db_port}{self.db_route}"
        data = {
            "id": request.request_id,
            "content": request.content
        }

        response = requests.post(server_http, json=data)
        data = response.json()

        return Response(status_code=response.status_code, content=data["content"])



if __name__ == "__main__":

    client: DatabaseServiceClient = DatabaseServiceClient()
    response = client.send_request(Request(request_id=1, params={}))
    print(response)
