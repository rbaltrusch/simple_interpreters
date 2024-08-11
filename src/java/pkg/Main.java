package pkg;

import java.util.Scanner;

/** A simple REPL interpreter */
public class Main {

    private static final String HELP = """
            A simple interpreter that can calculate arbitrarily nested mathematical expressions.
            It supports:
            - operators +, -, *, /, %.
            - variable assignments, e.g. x = 1 or x = 1 + 1
            - variable value retrieval
            - operation nesting
                """;

    /** Runs a new interpreter inside a REPL shell. */
    public static void main(String... args) {
        System.out.println("Simple REPL shell. Type \"help\" for help, or \"exit\" to exit the shell.");
        var interpreter = new Interpreter();
        try (var scanner = new Scanner(System.in)) {
            repl(scanner, interpreter);
        }
    }

    /** Runs the interpreter inside a REPL shell. */
    private static void repl(final Scanner scanner, final Interpreter interpreter) {
        while (true) {
            System.out.print(">> ");
            String input = scanner.nextLine();
            if (input.contains("exit"))
                break;
            if (input.contains("help")) {
                System.out.println(HELP);
                continue;
            }

            try {
                Double result = interpreter.interpret(input);
                if (result != null)
                    System.out.println(result);
            } catch (SyntaxException exc) {
                System.err.println(exc.getMessage());
            }
        }
    }
}
