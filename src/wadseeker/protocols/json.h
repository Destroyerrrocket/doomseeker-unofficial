/* Copyright 2011 Eeli Reilin. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL EELI REILIN OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The views and conclusions contained in the software and documentation
* are those of the authors and should not be interpreted as representing
* official policies, either expressed or implied, of Eeli Reilin.
*/

/**
* \file json.h
*/

#ifndef JSON_H
#define JSON_H

#include <QVariant>
#include <QString>

namespace QtJson
{

/**
* \enum JsonToken
*/
enum JsonToken
{
        JsonTokenNone = 0,
        JsonTokenCurlyOpen = 1,
        JsonTokenCurlyClose = 2,
        JsonTokenSquaredOpen = 3,
        JsonTokenSquaredClose = 4,
        JsonTokenColon = 5,
        JsonTokenComma = 6,
        JsonTokenString = 7,
        JsonTokenNumber = 8,
        JsonTokenTrue = 9,
        JsonTokenFalse = 10,
        JsonTokenNull = 11
};

/**
* \class Json
* \brief A JSON data parser
*
* Json parses a JSON data into a QVariant hierarchy.
*/
class Json
{
        public:
                /**
* Parse a JSON string
*
* \param json The JSON data
*/
                static QVariant parse(const QString &json);

                /**
* Parse a JSON string
*
* \param json The JSON data
* \param success The success of the parsing
*/
                static QVariant parse(const QString &json, bool &success);

                /**
* This method generates a textual JSON representation
*
* \param data The JSON data generated by the parser.
* \param success The success of the serialization
*/
                static QByteArray serialize(const QVariant &data);

                /**
* This method generates a textual JSON representation
*
* \param data The JSON data generated by the parser.
* \param success The success of the serialization
*
* \return QByteArray Textual JSON representation
*/
                static QByteArray serialize(const QVariant &data, bool &success);

        private:
                /**
* Parses a value starting from index
*
* \param json The JSON data
* \param index The start index
* \param success The success of the parse process
*
* \return QVariant The parsed value
*/
                static QVariant parseValue(const QString &json, int &index,
                                                                   bool &success);

                /**
* Parses an object starting from index
*
* \param json The JSON data
* \param index The start index
* \param success The success of the object parse
*
* \return QVariant The parsed object map
*/
                static QVariant parseObject(const QString &json, int &index,
                                                                           bool &success);

                /**
* Parses an array starting from index
*
* \param json The JSON data
* \param index The starting index
* \param success The success of the array parse
*
* \return QVariant The parsed variant array
*/
                static QVariant parseArray(const QString &json, int &index,
                                                                           bool &success);

                /**
* Parses a string starting from index
*
* \param json The JSON data
* \param index The starting index
* \param success The success of the string parse
*
* \return QVariant The parsed string
*/
                static QVariant parseString(const QString &json, int &index,
                                                                        bool &success);

                /**
* Parses a number starting from index
*
* \param json The JSON data
* \param index The starting index
*
* \return QVariant The parsed number
*/
                static QVariant parseNumber(const QString &json, int &index);

                /**
* Get the last index of a number starting from index
*
* \param json The JSON data
* \param index The starting index
*
* \return The last index of the number
*/
                static int lastIndexOfNumber(const QString &json, int index);

                /**
* Skip unwanted whitespace symbols starting from index
*
* \param json The JSON data
* \param index The start index
*/
                static void eatWhitespace(const QString &json, int &index);

                /**
* Check what token lies ahead
*
* \param json The JSON data
* \param index The starting index
*
* \return int The upcoming token
*/
                static int lookAhead(const QString &json, int index);

                /**
* Get the next JSON token
*
* \param json The JSON data
* \param index The starting index
*
* \return int The next JSON token
*/
                static int nextToken(const QString &json, int &index);
};


} //end namespace

#endif //JSON_H
