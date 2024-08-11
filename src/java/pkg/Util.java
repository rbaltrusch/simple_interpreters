package pkg;

import java.util.regex.Pattern;

public class Util {
    private static final Pattern numberPattern = Pattern.compile("^[0-9]*(\\.?[0-9]+)$");

    public static boolean isDouble(final String token) {
        return numberPattern.matcher(token).matches();
    }
}
