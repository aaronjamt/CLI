from .base import CanvasObject

import json
class QuizQuestion(CanvasObject):
	def __init__(self, submission, *args, **kwargs):
		self.submission = submission
		super().__init__(*args, *kwargs)

	def name(self):
		return self.question_name

	def text(self):
		return self.question_text

	# TODO
	def choices(self):
		""" Returns a list of answer choices in a human-readable format """
		answers = []
		if self.question_type == "multiple_choice_question":
			for option in self.answers:
				desc = None

				if desc is None and 'text' in option and len(option['text']) > 0:
					desc = option['text']

				if desc is None and 'html' in option and len(option['html']) > 0:
					desc = option['html']

				desc = "Unknown format: " + json.dumps(option)

				answers.append(f"[ID {self.id}] " + desc)
		else:
			answers.append("Unknown question type: " + json.dumps(self._attrs))

		return answers

	def set_answer(self, answer):
		self.answer = answer

	def get_answer(self):
		return {"id": self.id, "answer": self.answer}

class QuizSubmission(CanvasObject):
	def __init__(self, quiz, *args, **kwargs):
		self.quiz = quiz
		super().__init__(*args, *kwargs)

	def submit(self):
		data = {"attempt": self.attempt, "validation_token": self.validation_token, "access_code": self.quiz.access_code()}
		response = self._canvas._request(f"courses/{self.quiz.course_id}/quizzes/{self.quiz.id}/submissions/{self.id}/complete", method="POST", data=data)

	def questions(self):
		response = self._canvas._request(f"quiz_submissions/{self.id}/questions")['quiz_submission_questions']
		return [QuizQuestion(self, self._canvas, i) for i in response]

	def answer(self, answers):
		data = {
			"attempt": self.attempt,
			"validation_token": self.validation_token,
			"access_code": self.quiz.access_code(),
			"quiz_questions": [i.get_answer() for i in answers]
		}
		response = self._canvas._request(f"quiz_submissions/{self.id}/questions", method="POST", data=data)
		return response

class Quiz(CanvasObject):
	def __init__(self, course_id, *args, **kwargs):
		self.course_id = course_id

		super().__init__(*args, *kwargs)

		if 'has_access_code' in self._attrs and self.has_access_code:
			# Access code required, but not known
			self._access_code = None
			self.requires_access_code = True
		else:
			# No access code required
			self._access_code = False
			self.requires_access_code = False

	def access_code(self, access_code: str = None):
		""" Fetches and/or updates stored quiz access code """
		if self._access_code is False:
			if access_code is not None:
				print(f"Warning: Access code '{access_code}' provided for quiz which does not require an access code!")
			return None

		if self._access_code is None:
			if access_code is None:
				raise KeyError("No access code provided, but is required for quiz!")

			self._access_code = access_code

		if type(self._access_code) != str:
			raise TypeError(f"Invalid access code '{self._access_code}', should be a string!")

		return self._access_code

	def validate_access_code(self, code):
		if self._canvas._request(f"courses/{self.course_id}/quizzes/{self.id}/validate_access_code", method="POST", data={"access_code": code}):
			# Valid code, store for later
			self.access_code(code)
			return True
		# Invalid code
		return False

	def questions(self, quiz_submission: QuizSubmission = None):
		data = None
		if quiz_submission is not None:
			data = {"quiz_submission_id": quiz_submission.id, "quiz_submission_attempt": quiz_submission.attempt}
		response = self._canvas._request(f"courses/{self.course_id}/quizzes/{self.id}/questions", data=data)
		return response

	def start_quiz(self, access_code: str = None):
		data = {"access_code": access_code} if self.access_code(access_code) else None
		response = self._canvas._request(f"courses/{self.course_id}/quizzes/{self.id}/submissions", method="POST", data=data)
		return QuizSubmission(self, self._canvas, response["quiz_submissions"][0])

	def get_quiz_submission(self):
		response = self._canvas._request(f"courses/{self.course_id}/quizzes/{self.id}/submission")
		return QuizSubmission(self, self._canvas, response["quiz_submissions"][0])
