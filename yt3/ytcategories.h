/* $BEGIN_LICENSE

This file is part of Minitube.
Copyright 2009, Flavio Tordini <flavio.tordini@gmail.com>

Minitube is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Minitube is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Minitube.  If not, see <http://www.gnu.org/licenses/>.

$END_LICENSE */

#ifndef YTCATEGORIES_H
#define YTCATEGORIES_H

#include <QtNetwork>

struct YTCategory {
    QString term;
    QString label;
};

class YTCategories : public QObject {

    Q_OBJECT

public:
    YTCategories(QObject *parent = 0);
    void loadCategories(QString language = QString());
    
signals:
    void categoriesLoaded(const QVector<YTCategory> &);
    void error(QString message);

private slots:
    void parseCategories(const QByteArray &bytes);
    void requestError(const QString &message);

private:
    QString lastLanguage;

};

#endif // YTCATEGORIES_H
