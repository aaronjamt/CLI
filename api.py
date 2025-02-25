from .objects.courses import Course

import requests

class Canvas:
	""" Base Canvas API wrapper """

	def __init__(self, base_url, access_token):
		""" Initializes the Canvas API wrapper.
		base_url = The base URL of your Canvas instance
		access_token = Access token generated on your Canvas account settings page
		"""

		self.base_url = base_url
		self.headers = {
			"Authorization": "Bearer {}".format(access_token)
		}

	def _request(self, endpoint, method='GET', headers=None, data=None, endpoint_is_url=False):
		""" Makes a request to the Canvas API with optional headers and data
		If `endpoint_is_url` is True, `endpoint` is treated as a raw URL, rather than an endpoint
		"""

		# Combine any provided headers with the class-wide headers
		if headers is None:
			headers = {}
		headers.update(self.headers)

		# Do request
		if endpoint_is_url:
			url = endpoint
		else:
			url = "{}/api/v1/{}".format(self.base_url, endpoint)

		# If it's a POST requests, treat data as JSON
		try:
			if method == "POST":
				resp = requests.request(method, url, headers=headers, json=data, timeout=5)
			else:
				resp = requests.request(method, url, headers=headers, data=data, timeout=5)
		except:
			print(f"Failed Canvas API request with method='{method}' to URL '{url}', using headers='{headers}' and data='{data}'")
			raise Exception()

		if resp.status_code == 200:
			result = resp.json()
			if method == "GET":
				for header in resp.headers:
					if header.upper() == "LINK":
						# Need to handle pagination
						pages = resp.headers[header].split(",")
						# If there's a next page, fetch it
						for page in pages:
							url, type = page.split(";")
							# Extract the URL from <>
							if url.startswith('<') and url.endswith('>'):
								url = url[1:-1]

							if 'rel="next"' in type:
								# This is the next page, fetch it and add to our results
								result += self._request(url, method=method, headers=headers, data=data, endpoint_is_url=True)

			#print("API Cost:",resp.headers["X-Request-Cost"])
			#print("API RL Remaining:",resp.headers["X-Rate-Limit-Remaining"])

			return result
		else:
			raise ValueError("Invalid response code from Canvas: got HTTP {} with body during {} to '{}':\n{}".format(resp.status_code, method, endpoint, resp.text))

	def courses(self):
		""" Fetches a list of all courses """
		courses = self._request("courses")
		return [Course(self, i) for i in courses]

	def course(self, id):
		""" Fetches a course by ID """
		return Course(self, self._request(f"courses/{id}"))
