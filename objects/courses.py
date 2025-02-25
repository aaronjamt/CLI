from .base import CanvasObject
from .assignments import Assignment
from .quizzes import Quiz
from datetime import datetime

class Term(CanvasObject):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)

		if self._attrs["start_at"] is not None:
			self._attrs["start_at"] = datetime.fromisoformat(self._attrs["start_at"])
		if self._attrs["end_at"] is not None:
			self._attrs["end_at"] = datetime.fromisoformat(self._attrs["start_at"])

class Course(CanvasObject):
	def assignments(self):
		""" Fetches all available assignments (which includes quizzes) for this course """
		assignments = self._canvas._request(f"courses/{self.id}/assignments")
		result = []
		for i in assignments:
			# Some assignments might be quizzes, which should be handled differently
			if i["is_quiz_assignment"]:
				result.append(Quiz(self.id, self._canvas, i))
			else:
				result.append(Assignment(self.id, self._canvas, i))
		return result

	def quizzes(self) -> list[Quiz, ...]:
		""" Fetches all available quizzes for this course """
		try:
			# Fetch from quizzes endpoint
			quizzes = self._canvas._request(f"courses/{self.id}/quizzes")
			# Convert JSON to Quiz objects
			return [Quiz(self.id, self._canvas, i) for i in quizzes]
		except ValueError:
			# If the quizzes tab is unavailable, fall back to the regular assignments endpoint and filter
			print("Warning: Unable to access quizzes tab, falling back to assignments tab")
			assignments = self.assignments()
			# Filter only Quiz objects
			return [i for i in assignments if type(i) == Quiz]
