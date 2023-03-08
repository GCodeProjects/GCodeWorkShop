
QString removeZeros(QString str)
{
    QRegularExpression regex;
    regex.setPattern("[\\d]{0,}[-.]{0,1}[-+.0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$"); //[\\d]+[.][-+.0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$"
    auto match = regex.match(str);

    while (match.hasMatch()) {
        int pos = match.capturedStart();

        if ((str.at(match.capturedEnd() - 1) == '0')) {
            str.remove(match.capturedEnd() - 1, 1);
        } else {
            pos = match.capturedEnd();
        }

        match = regex.match(str, pos);
    }

    if (str == "-0.") {
        str = "0.";
    }

    return (str);
}
