package pkg;

/** To be thrown for invalid interpreter input. */
public class SyntaxException extends RuntimeException {
    public SyntaxException(final String message) {
        super(message);
    }
}
