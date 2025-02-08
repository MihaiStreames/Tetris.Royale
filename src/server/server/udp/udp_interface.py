
import socket
import json
from abc import ABC, abstractmethod
from request_objects import Request, Response, dict_to_json, content_to_dict



class UDPServerInterface(ABC):

    def __init__(self, host="127.0.0.1", port=12345, BUFFER_SIZE: int = 1024) -> None:

        self.host: str = host
        self.port: int = port
        self.BUFFER_SIZE: int = BUFFER_SIZE
        self.server_socket: socket.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.server_socket.bind((self.host, self.port))

    def listen_for_requests(self) -> None:

        print(f"[!] Server listening on {self.host}:{self.port}")

        while True:
            data, client_address = self.server_socket.recvfrom(self.BUFFER_SIZE)
            print(f"[event] Request from {client_address} with data={data}")  # remove this line later
            self.handle_request(data=data, client_address=client_address)

    def handle_request(self, data: bytes, client_address: tuple) -> None:

        try:

            message: dict | any = json.loads(data.decode())

            if "id" not in message or "content" not in message:
                response: dict = {"content": "Missing 'id' or 'content' field"}
                self.send_response(content=response, client_address=client_address)
                return
            
            # Create a Request object from the incoming message
            request_object: Request = Request(request_id=int(message["id"]), params=content_to_dict(message["content"]))
            # Process the request
            response_object: Response = self.process_request(request=request_object)
            # Send the response back to the client
            self.send_response(content=response_object.parsed_content, client_address=client_address)

        except Exception as e:
            error_response: dict = {"content": str(e)}
            self.send_response(content=error_response, client_address=client_address)

    def send_response(self, content: dict, client_address: tuple) -> None:

        try:
            response_data = dict_to_json(content).encode()
            self.server_socket.sendto(response_data, client_address)

        except Exception as e:
            print(f"[ERROR] Failed to send response to client {client_address} : {e}")

    @abstractmethod
    def process_request(self, request: Request) -> Response:
        ...

