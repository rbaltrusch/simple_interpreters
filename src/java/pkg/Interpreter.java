package pkg;

//import static pkg.Util.isDouble;

import java.util.List;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.Map;
import java.util.HashMap;

/**
 * A simple interpreter that can calculate arbitrarily nested mathematical
 * expressions.
 * It supports:
 * - operators +, -, *, /, %.
 * - variable assignments, e.g. x = 1 or x = 1 + 1
 * - variable value retrieval
 * - operation nesting
 */
public class Interpreter {

    private final Map<String, Double> variables = new HashMap<>();
    private static final String OPERATORS = "+-*/%";

    /**
     * Tokenizes and interprets the specified string, supporting variable assignment
     * and arbitrarily nested mathematical expressions.
     * Throws a SyntaxException for invalid input.
     */
    public Double interpret(final String string) {
        List<String> tokens = Tokenizer.tokenize(string);
        if (!isBalancedParentheses(tokens))
            throw new SyntaxException("Unbalanced parentheses.");
        Double result = interpret(tokens);
        return result;
    }

    /** @see Interpreter#interpret(String) */
    public Double interpret(final List<String> tokens) {
        if (tokens.isEmpty()) {
            return null;
        }

        // check for assignment
        if (tokens.size() > 2 && tokens.get(1).equals("=")) {
            String variable = tokens.remove(0);
            tokens.remove(0); // assignment operator
            double value = interpret(tokens);
            variables.put(variable, value);
            return value;
        }

        Expression root = new Expression();
        Expression current = root;
        while (tokens.size() > 0) {
            if (current.isSatisfied()) {
                if (current.isLowPrecedence()) {
                    current = current.createSubExpression();
                } else {
                    root = new Expression(current);
                    current = root;
                }
            }

            String token = tokens.remove(0);
            if (token.equals(")")) {
                return root.getValue();
            }
            if (token.equals("(")) {
                current.pushValue(interpret(tokens));
            } else if (Util.isDouble(token)) {
                current.pushValue(Double.parseDouble(token));
            } else if (OPERATORS.contains(token)) {
                current.setOperator(token);
            } else {
                try {
                    current.pushValue(variables.get(token));
                } catch (Exception exc) {
                    throw new RuntimeException("Undefined variable " + token);
                }
            }
        }

        return root.getValue();
    }

    private static boolean isBalancedParentheses(final List<String> tokens) {
        int scopes = 0;
        for (var token : tokens) {
            if (token == "(")
                scopes++;
            else if (token == ")")
                scopes--;
        }
        return scopes == 0;
    }
}
