"""Contains an implementation of a simple interpreter supporting variables
and arbitrarily nested mathematical operations.

Note: does not throw Syntax errors for unclosed scopes.
"""

import re
from operator import add, sub, mul, truediv, mod
from typing import List, Union

OPERATORS = {"+": add, "-": sub, "*": mul, "/": truediv, "%": mod}
TOKENIZER = re.compile(
    r"\s*(=>|[-+*\/\%=\(\)]|[A-Za-z_][A-Za-z0-9_]*|[0-9]*\.?[0-9]+)\s*"
)

Expression = List[Union[str, float, "Expression"]]


def try_float(x: str) -> str | float:
    """Tries to cast x to float and returns it if possible, else returns x unchanged."""
    try:
        return float(x)
    except Exception:  # pylint: disable=broad-except
        return x


def tokenize(expression: str) -> List[Expression]:
    """Parses a (possibly nested) Expression from the specified string, containing:
    - a list in place of scoped calculations e.g. (1 + 1) -> [[1, '+', 1]]
    - a string for parsed operators or variables
    - a float for parsed numbers
    """
    if expression == "":
        return []

    tokens = (try_float(s) for s in TOKENIZER.findall(expression) if not str.isspace(s))
    scopes = [[]]
    for token in tokens:
        match token:
            case "(":
                scopes.append([])
            case ")":
                scope = scopes.pop()
                scopes[-1].append(scope)
            case _:
                scopes[-1].append(token)

    if len(scopes) > 1:
        raise InvalidInputException("Imbalanced parentheses")
    return scopes[0]


class InvalidInputException(Exception):
    """To be raised for invalid input to the interpreter"""

class UndefinedVariableException(InvalidInputException):
    """To be raised for undefined variables being referenced"""

class Interpreter:
    """A simple interpreter that can calculate arbitrarily nested mathematical expressions.
    It supports:
    - operators +, -, *, /, %.
    - variable assignments, e.g. x = 1 or x = 1 + 1
    - variable value retrieval
    - operation nesting
    """

    def __init__(self):
        self.variables = {}

    def get_variable(self, name: str) -> float:
        """Returns the value of the specified variable or throws UndefinedVariableException"""
        try:
            return self.variables[name]
        except KeyError:
            raise UndefinedVariableException(f"Undefined variable {name}") from None

    def interpret(self, expression: str) -> float | None:
        """Parses the specified string expression and returns the calculated result.
        - Supports variable assignment (yielding the assigned value)
        - Supports arbitrarily nested mathematical expressions
        - Result is either the float result or None for empty expressions.
        Throws:
        - KeyError for undefined variables
        - InvalidInputException if the expression is unparsable.
        """

        try:
            tokens = tokenize(expression)
        except Exception: # pylint: disable=broad-except
            raise InvalidInputException("Invalid input: failed to tokenize.") from None

        match tokens:
            case []:
                return None
            case [str()]:
                return self.get_variable(tokens[0])
            case [str(), "=", *rest]:
                result = self.calculate(rest)
                self.variables[tokens[0]] = result
                return result
            case _:
                return self.calculate(tokens)

    def calculate(self, tokens: List[Expression]) -> float:
        """Calculates the result of the specified token expression and returns it."""
        match tokens:
            case [float()]:
                return tokens[0]
            case [list(), *rest]:
                return self.calculate([self.calculate(tokens[0])] + rest)
            case [str(), *rest]:
                return self.calculate([self.get_variable(tokens[0])] + rest)
            case [float(), "*" | "/" | "%", float(), *_]:
                left, op, right, *rest = tokens
                return self.calculate([OPERATORS[op](left, right)] + rest)
            case [float(), str(), *rest]:
                return OPERATORS[tokens[1]](tokens[0], self.calculate(rest))
            case _:
                raise InvalidInputException("Invalid input.")
