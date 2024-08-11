package pkg;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Tokenizer {

    private static final Pattern tokenPattern = Pattern.compile(
            "=>|[-+*/%=\\(\\)]|[A-Za-z_][A-Za-z0-9_]*|[0-9]*(\\.?[0-9]+)");

    public static List<String> tokenize(final String input) {
        List<String> tokens = new ArrayList<>();
        Matcher matcher = tokenPattern.matcher(input);
        while (matcher.find()) {
            tokens.add(matcher.group());
        }
        return tokens;
    }
}
