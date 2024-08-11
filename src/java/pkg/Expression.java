package pkg;

import java.util.Objects;
import java.util.Set;
import java.util.function.BiFunction;

/**
 * Encapsulates a bi-operator taking two operands. Can optionally evaluate to
 * just a literal if no operator is specified.
 */
public class Expression implements Valued {

    private Valued lhs;
    private Valued rhs;
    private BiFunction<Double, Double, Double> operator;
    private String operatorToken;

    public Expression() {
    }

    public Expression(final Valued lhs) {
        this.lhs = lhs;
    }

    /**
     * Regroups the right-hand side of this into another expression, then returns
     * it.
     * Useful for when higher-precedence operators follow the rhs after already
     * parsing it.
     */
    public Expression createSubExpression() {
        Expression expression = new Expression(rhs);
        rhs = expression;
        return expression;
    }

    /**
     * Sets the operator of this, or throws a SyntaxException for invalid operators.
     */
    public void setOperator(final String token) {
        if (operator != null)
            throw new SyntaxException("Invalid syntax: another operator was not expected.");
        operator = determineOperator(token);
        operatorToken = token;
    }

    private BiFunction<Double, Double, Double> determineOperator(final String token) {
        switch (token) {
            case "+":
                return (x, y) -> x + y;
            case "-":
                return (x, y) -> x - y;
            case "*":
                return (x, y) -> x * y;
            case "/":
                return (x, y) -> x / y;
            case "%":
                return (x, y) -> x % y;
            default:
                throw new SyntaxException(String.format("Invalid operator: '%s'.", token));
        }
    }

    /** Sets lhs or rhs to value, depending on */
    public void pushValue(final Double value) {
        if (value == null)
            throw new SyntaxException("Invalid input: missing value.");
        pushValue(new Literal(value));
    }

    /** @see Expression#pushValue(double) */
    public void pushValue(final Valued valued) {
        if (lhs == null)
            lhs = valued;
        else if (rhs == null)
            rhs = valued;
        else
            throw new SyntaxException("Invalid input: did not expect additional value.");
    }

    /**
     * Returns true if this Expression contains a full set of tokens
     * (operation and 2 operands).
     */
    public boolean isSatisfied() {
        return rhs != null;
    }

    /** Returns true if this is an operation of lower precedence. */
    public boolean isLowPrecedence() {
        return Set.of("+", "-").contains(operatorToken);
    }

    /**
     * Calculates the value of this Expression:
     * - for simple expressions without operators, returns lhs,
     * - for expressions with operators, applies it to lhs and rhs, then returns the
     * result,
     * - else throws a SyntaxException.
     */
    @Override
    public double getValue() {
        if (lhs == null) {
            throw new SyntaxException("Invalid syntax: missing value.");
        }
        if (rhs == null) {
            if (operator != null) {
                String template = "Invalid syntax: missing second operand for operator '%s'.";
                String message = String.format(template, this.operatorToken);
                throw new SyntaxException(message);
            }
            return lhs.getValue();
        }
        if (operator == null) {
            throw new SyntaxException("Invalid syntax: missing operator.");
        }
        return operator.apply(lhs.getValue(), rhs.getValue());
    }
}
