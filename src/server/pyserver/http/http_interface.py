

from flask import Flask, request, jsonify
from abc import abstractmethod, ABC
from ..request_objects import Request, Response, content_to_dict



class HTTPServerInterface(ABC):

    """
    HTTPServerInterface is an abstract class that defines the interface for an HTTP server.
    By default, the server will run on http://, as well as using the default port 5000.
    The route is set to '/api/requests' by default.
    HTTPServerInterface will handle failed requests and return a 400 error if the request does not contain the required fields.
    Error 500 will be returned if there is an exception while processing the request.
    This should be handled by the user, but for now this will do.
    """

    def __init__(self, host="127.0.0.1", port=5000, route: str = '/api/requests') -> None:
       
        self.app: Flask = Flask(__name__)
        self.host: str = host
        self.port: int = port
        self.route: str = route
        self.setup_routes()

    def setup_routes(self) -> None:

        @self.app.route(self.route, methods=["POST"])
        def handle_request() -> str:

            try:

                data: dict = request.get_json()

                if "id" not in data or "content" not in data:
                    # Return a 400 error if the request does not contain the required fields
                    return jsonify({"content": "Missing 'id' or 'content' field"}), 400

                request_object: Request = Request(request_id=int(data["id"]), params=content_to_dict(data["content"]))
                response: Response = self.process_request(request=request_object)
                
                return jsonify({
                    "content": response.parsed_content,
                }), response.status_code

            except Exception as e:
                return jsonify({
                    "content": str(e),
                }), 500

    @abstractmethod
    def process_request(self, request: Request) -> Response:
        ...

    def run(self):
        print(f" * Server starting on http://{self.host}:{self.port}")
        self.app.run(host=self.host, port=self.port, debug=False)
        
