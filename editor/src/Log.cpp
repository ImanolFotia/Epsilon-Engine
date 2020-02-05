#include <include/Log.hpp>
#include <ui_leveleditor.h>

std::vector<Line> Log::mLines;
std::vector<std::string> Log::mFormatedLines;
Log* Log::mInstance;
Ui_LevelEditor* Log::Ui_Ref;

std::string Log::println(std::string tag, std::string content) {
    //Format as html
    std::string p = "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">" \
                    "<span style=\" font-family:'MS Shell Dlg 2'; font-size:13px; font-weight:600; color:#0000ff;\">" +
                    tag + ": </span><span style=\" font-family:'MS Shell Dlg 2'; font-size:13px;\">" +
                    content + "</span><span style=\" font-family:'MS Shell Dlg 2';\"><br /></span><span style=\" font-family:'MS Shell Dlg 2'; font-size:13px; font-weight:600; color:#ff0000;\"></p>";
    Ui_Ref->textEdit->insertHtml(QString(p.c_str()));
    Line line(tag, content);
    mLines.push_back(line);
    mFormatedLines.push_back(p);
    return p;
}

void Log::setUiReference(Ui_LevelEditor* ref){
    Ui_Ref = ref;
}

Ui_LevelEditor* Log::getUiReference() {
    return Ui_Ref;
}
