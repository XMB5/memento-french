////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef GRAPHICUTILS_H
#define GRAPHICUTILS_H

#include <QString>

class GraphicUtils
{
public:
    static QString generatePitchGraph(const int      moraSize,
                                      const uint8_t  pos, 
                                      const QString &fill, 
                                      const QString &stroke);

private:
    GraphicUtils() {}
};

#endif // GRAPHICUTILS_H