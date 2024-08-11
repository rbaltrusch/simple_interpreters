"""A simple REPL interpreter."""

from interpreter.interpreter import Interpreter, InvalidInputException

_interpreter = Interpreter()
print('Simple REPL shell. Type "help" for help, or "exit" to exit the shell.')
while True:
    try:
        input_expression = input(">> ")
    except KeyboardInterrupt:
        break

    if "help" in input_expression:
        print(help(Interpreter))
        continue

    if "exit" in input_expression:
        break

    try:
        result = _interpreter.interpret(input_expression)
        if result is not None:
            print(result)
    except InvalidInputException as exc:
        print(str(exc))
