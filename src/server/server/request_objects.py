
import json
from typing import Optional, Dict


def content_to_dict(content: str) -> Optional[Dict]:
    """
    Converts a string to a JSON object. 
    If the string is not a valid JSON object, a ValueError is raised.
    """
    try:
        return json.loads(content)
    except json.JSONDecodeError as e:
        raise ValueError(f"Invalid JSON: {e}") from e


def dict_to_json(content: Dict) -> Optional[str]:
    """
    Converts a dictionary to a JSON string.
    If there is an error while converting, raises a ValueError.
    """
    try:
        return json.dumps(content)
    except (TypeError, ValueError) as e:
        raise ValueError(f"Invalid data for JSON serialization: {e}") from e


class Request:
    
    def __init__(self, request_id: int, params: Dict) -> None:

        """
        Initializes a Request object.
        
        :param request_id: ID associated with the request
        :param params: Dictionary containing the request parameters
        """

        self._request_id: int = request_id
        self._params: Dict = params
        self._content: str = dict_to_json(params)

    @property
    def request_id(self) -> int:
        return self._request_id

    @property
    def params(self) -> Dict:
        return self._params

    @property
    def content(self) -> str:
        return self._content

    def __repr__(self) -> str:
        return f"Request with id [{self.request_id}] & params={self.params}"



class Response:

    def __init__(self, status_code: int, content: Dict) -> None:
        
        """
        Initializes a Response object.

        :param status_code: HTTP status code of the response
        :param content: Dictionary containing the response content
        """

        self._status_code: int = status_code
        self._content: Dict = content
        self._parsed: Optional[Dict] = dict_to_json(content)
    
    @property
    def status_code(self) -> int:
        return self._status_code

    @property
    def content(self) -> Dict:
        return self._content

    @property
    def parsed_content(self) -> Dict:
        return self._parsed

    def __repr__(self) -> str:
        return f"Response with status code [{self.status_code}] & content={self.content}"
    
