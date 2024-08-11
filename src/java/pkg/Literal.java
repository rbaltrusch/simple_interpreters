package pkg;

/** A literal value that always evaluates to itself. */
public class Literal implements Valued {

    private double value;

    public Literal(final double value) {
        this.value = value;
    }

    @Override
    public double getValue() {
        return value;
    }
}
