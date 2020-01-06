/*
 * Copyright (c) 2014-2020 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * QPlainTextEdit markdown highlighter
 */


#pragma once

#include <QTextCharFormat>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;

QT_END_NAMESPACE

class MarkdownHighlighter : public QSyntaxHighlighter
{
Q_OBJECT

public:
    enum HighlightingOption{
        None = 0,
        FullyHighlightedBlockQuote = 0x01
    };
    Q_DECLARE_FLAGS(HighlightingOptions, HighlightingOption)

    MarkdownHighlighter(QTextDocument *parent = nullptr,
                        HighlightingOptions highlightingOptions =
                        HighlightingOption::None);

    inline QColor codeBlockBackgroundColor() const {
        const QBrush brush = _formats[CodeBlock].background();

        if (!brush.isOpaque()) {
            return QColor(Qt::transparent);
        }

        return brush.color();
    }

   /**
    * @brief returns true if c is octal
    * @param c the char being checked
    * @returns true if the number is octal, false otherwise
    */
   inline bool isOctal(const char c) {
       return (c >= '0' && c <= '7');
   }

   /**
    * @brief returns true if c is hex
    * @param c the char being checked
    * @returns true if the number is hex, false otherwise
    */
   inline bool isHex(const char c) {
       return (
           (c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F'));
   }

    // we use some predefined numbers here to be compatible with
    // the peg-markdown parser
    enum HighlighterState {
        NoState = -1,
        Link = 0,
        Image = 3,
        CodeBlock,
        CodeBlockComment,
        Italic = 7,
        Bold,
        List,
        Comment = 11,
        H1,
        H2,
        H3,
        H4,
        H5,
        H6,
        BlockQuote,
        HorizontalRuler = 21,
        Table,
        InlineCodeBlock,
        MaskedSyntax,
        CurrentLineBackgroundColor,
        BrokenLink,
        FrontmatterBlock,
        TrailingSpace,
        CheckBoxUnChecked,
        CheckBoxChecked,

        //code highlighting
        CodeKeyWord = 1000,
        CodeString = 1001,
        CodeComment = 1002,
        CodeType = 1003,
        CodeOther = 1004,
        CodeNumLiteral = 1005,
        CodeBuiltIn = 1006,

        // internal
        CodeBlockEnd = 100,
        HeadlineEnd,
        FrontmatterBlockEnd,

        //languages
        /*********
         * When adding a language make sure that its value is a multiple of 2
         * This is because we use the next number as comment for that language
         * In case the language doesn't support multiline comments in the traditional C++
         * sense, leave the next value empty. Otherwise mark the next value as comment for
         * that language.
         * e.g
         * CodeCpp = 200
         * CodeCppComment = 201
         */
        CodeCpp = 200,
        CodeCppComment = 201,
        CodeJs = 202,
        CodeJsComment = 203,
        CodeC = 204,
        CodeCComment = 205,
        CodeBash = 206,
        CodePHP = 208,
        CodePHPComment = 209,
        CodeQML = 210,
        CodeQMLComment = 211,
        CodePython = 212,
        CodeRust = 214,
        CodeRustComment = 215,
        CodeJava = 216,
        CodeJavaComment = 217,
        CodeCSharp = 218,
        CodeCSharpComment = 219,
        CodeGo = 220,
        CodeGoComment = 221,
        CodeV = 222,
        CodeVComment = 223,
        CodeSQL = 224,
        CodeJSON = 226,
        CodeXML = 228,
        CodeCSS = 230,
        CodeCSSComment = 231,
        CodeTypeScript = 232,
        CodeTypeScriptComment = 233,
        CodeYAML = 234,
        CodeINI = 236,
        CodeTaggerScript = 238,
        CodeVex = 240,
        CodeVexComment = 241,
    };
    Q_ENUMS(HighlighterState)

//    enum BlockState {
//        NoBlockState = 0,
//        H1,
//        H2,
//        H3,
//        Table,
//        CodeBlock,
//        CodeBlockEnd
//    };

    void setTextFormats(QHash<HighlighterState, QTextCharFormat> formats);
    void setTextFormat(HighlighterState state, QTextCharFormat format);
    void clearDirtyBlocks();
    void setHighlightingOptions(const HighlightingOptions options);
    void initHighlightingRules();
signals:
    void highlightingFinished();

protected slots:
    void timerTick();

protected:
    struct HighlightingRule {
        explicit HighlightingRule(const HighlighterState state_) : state(state_) {}
        HighlightingRule() = default;

        QRegularExpression pattern;
        HighlighterState state = NoState;
        /*
         * waqar144:
         * Dear programmer,
         * shouldContain[3] is an array of 3 and it may seem that if we don't use an array here
         * but a simple string (since most of the rules have only one check), it will be faster.
         * It won't be faster. It will be slower.
         * The resulting struct is larger in size - 40, as opposed to 24 with a single string.
         * Dated: 5-Jan-2020
         */
        QString shouldContain[3];
        uint8_t capturingGroup = 0;
        uint8_t maskedGroup = 0;
        bool useStateAsCurrentBlockState = false;
        bool disableIfCurrentStateIsSet = false;
    };

    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

    void initTextFormats(int defaultFontSize = 12);

    void highlightMarkdown(const QString& text);

    void highlightHeadline(const QString& text);

    void highlightSubHeadline(const QString &text, HighlighterState state);

    void highlightAdditionalRules(const QVector<HighlightingRule> &rules,
                                  const QString& text);

    void highlightCodeBlock(const QString& text);

    void highlightSyntax(const QString &text);

    int highlightNumericLiterals(const QString& text, int i);

    int highlightStringLiterals(QChar strType, const QString& text, int i);

    void ymlHighlighter(const QString &text);

    void iniHighlighter(const QString &text);

    void cssHighlighter(const QString &text);

    void xmlHighlighter(const QString &text);

    void taggerScriptHighlighter(const QString &text);

    void highlightFrontmatterBlock(const QString& text);

    void highlightCommentBlock(QString text);

    void addDirtyBlock(const QTextBlock& block);

    void reHighlightDirtyBlocks();

    void setHeadingStyles(MarkdownHighlighter::HighlighterState rule,
                     const QRegularExpressionMatch &match,
                     const int capturedGroup);

    static void initCodeLangs();

    QVector<HighlightingRule> _highlightingRulesPre;
    QVector<HighlightingRule> _highlightingRulesAfter;
    static QHash<QString, HighlighterState> _langStringToEnum;
    QVector<QTextBlock> _dirtyTextBlocks;
    QHash<HighlighterState, QTextCharFormat> _formats;
    QTimer *_timer;
    bool _highlightingFinished;
    HighlightingOptions _highlightingOptions;

    void setCurrentBlockMargin(HighlighterState state);
};
