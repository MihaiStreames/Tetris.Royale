

from .http_interface import HTTPServerInterface
from ..request_objects import Request, Response


class DatabaseServer(HTTPServerInterface):

    def __init__(self, host="127.0.0.1", port=5000, route = '/db/requests'):
        super().__init__(host=host, port=port, route=route)

    def process_request(self, request: Request) -> Response:

        """

        Processes the request and returns a response.
        This needs to be implemented to handle the specific request logic.
        Connect this to the database to process the request accordingly.

        Please note that content is a dictionary, but IS optional.
        You shouldn't use it if you don't need it, i.e. if 404.
        Code 200 is for successful requests.

        """

        # TODO : Implement the logic to process the request
        # Feel free to call abstractmethods for each request ID and
        # then inherit this class to implement the logic for each request ID,
        # using the database to process the request accordingly.
        
        match request.request_id:

            case 1: return Response(status_code=200, content={})
            case 2: return Response(status_code=200, content={})
            # ...
            case _: return Response(status_code=404, content={})
        
