#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QObject>
#include <QDebug>
#include <cmath>
#include <vector>
#include <map>
#include <tuple>
#include <queue>
#include <limits>

class Navigator : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentFloor READ currentFloor WRITE setCurrentFloor NOTIFY currentFloorChanged)
    Q_PROPERTY(QString stepByStepRoute READ stepByStepRoute NOTIFY stepByStepRouteChanged)

public:
    Navigator(QObject *parent = nullptr) : QObject(parent) {
        coordinates = {
                       {"101H", {500, 1600, 1}}, {"102H", {580, 1680, 1}}, {"103H", {700, 1680, 1}},
                       {"104H", {770, 1485, 1}}, {"105H", {1125, 1700, 1}}, {"106H", {1300, 1700, 1}},
                       {"108H", {1800, 1630, 1}}, {"109H", {1830, 1080, 1}}, {"110H", {1830, 850, 1}},
                       {"111H", {1520, 700, 1}}, {"112H", {1670, 250, 1}}, {"113H", {760, 250, 1}},
                       {"114H", {450, 250, 1}}, {"115H", {120, 350, 1}},
                       {"101H_exit", {600, 1600, 1}}, {"102H_exit", {580, 1600, 1}}, {"103H_exit", {700, 1600, 1}},
                       {"104H_exit", {850, 1485, 1}}, {"105H_exit", {1125, 1600, 1}}, {"106H_exit", {1300, 1600, 1}},
                       {"108H_exit", {1700, 1600, 1}}, {"109H_exit", {1700, 1080, 1}}, {"110H_exit", {1700, 850, 1}},
                       {"111H_exit", {1520, 850, 1}}, {"112H_exit", {1670, 350, 1}}, {"113H_exit", {760, 350, 1}},
                       {"114H_exit", {450, 350, 1}}, {"115H_exit", {200, 350, 1}},

                       {"201H", {500, 1600, 2}}, {"202H", {580, 1680, 2}}, {"203H", {700, 1680, 2}},
                       {"215H", {260, 250, 2}}, {"205H", {1125, 1700, 2}}, {"206H", {1300, 1700, 2}}, {"207H", {1550, 1800, 2}},
                       {"208H", {1800, 1630, 2}}, {"209H", {1830, 1080, 2}}, {"210H", {1830, 850, 2}},
                       {"211H", {1520, 700, 2}}, {"212H", {1670, 250, 2}}, {"213H", {760, 250, 2}},
                       {"214H", {450, 250, 2}}, {"116H", {80, 350, 2}},
                       {"201H_exit", {600, 1600, 2}}, {"202H_exit", {580, 1600, 2}}, {"203H_exit", {700, 1600, 2}},
                       {"215H_exit", {260, 350, 2}}, {"205H_exit", {1125, 1600, 2}}, {"206H_exit", {1300, 1600, 2}},{"207H_exit", {1550, 1600, 2}},
                       {"208H_exit", {1700, 1600, 2}}, {"209H_exit", {1700, 1080, 2}}, {"210H_exit", {1700, 850, 2}},
                       {"211H_exit", {1520, 850, 2}}, {"212H_exit", {1670, 350, 2}}, {"213H_exit", {760, 350, 2}},
                       {"214H_exit", {450, 350, 2}}, {"216H_exit", {970, 1680, 2}},

                       {"301H", {500, 1600, 3}}, {"302H", {580, 1680, 3}}, {"303H", {700, 1680, 3}},
                       {"304H", {770, 1485, 3}}, {"305H", {1125, 1700, 3}}, {"306H", {1300, 1700, 3}},{"307H", {1550, 1800, 3}},
                       {"308H", {1800, 1630, 3}}, {"309H", {1830, 1080, 3}}, {"310H", {1830, 850, 3}},
                       {"311H", {1520, 700, 3}}, {"312H", {1670, 250, 3}}, {"313H", {760, 250, 3}},
                       {"314H", {450, 250, 3}}, {"315H", {80, 350, 3}},
                       {"301H_exit", {600, 1600, 3}}, {"302H_exit", {580, 1600, 3}}, {"303H_exit", {700, 1600, 3}},
                       {"304H_exit", {850, 1485, 3}}, {"305H_exit", {1125, 1600, 3}}, {"306H_exit", {1300, 1600, 3}},{"307H_exit", {1550, 1600, 3}},
                       {"308H_exit", {1700, 1600, 3}}, {"309H_exit", {1700, 1080, 3}}, {"310H_exit", {1700, 850, 3}},
                       {"311H_exit", {1520, 850, 3}}, {"312H_exit", {1670, 350, 3}}, {"313H_exit", {760, 350, 3}},
                       {"314H_exit", {450, 350, 3}}, {"315H_exit", {200, 350, 3}},

                       {"staircase", {900, 1600, 1}},{"staircase2", {900, 1600, 2}}, {"staircase3", {900, 1600, 3}},{"Главный вход", {1550, 1750, 1}},
                       {"staircase11", {1700, 1500, 1}},{"staircase12", {1700, 1500, 2}}, {"staircase13", {1700, 1500, 3}},
                       {"staircase21", {480, 350, 1}},{"staircase22", {480, 350, 2}}, {"staircase23", {480, 350, 3}}

                       };

        graph = {
                 {"101H", {"101H_exit"}}, {"101H_exit", {"101H", "102H_exit"}},
                 {"102H", {"102H_exit"}}, {"102H_exit", {"102H", "101H_exit","103H_exit"}},
                 {"103H", {"103H_exit"}}, {"103H_exit", {"103H", "staircase","102H_exit","105H_exit"}},
                 {"104H", {"104H_exit"}}, {"104H_exit", {"104H", "staircase"}},
                 {"105H", {"105H_exit"}}, {"105H_exit", {"105H", "staircase","106H_exit","103H_exit"}},
                 {"106H", {"106H_exit"}}, {"106H_exit", {"106H","105H_exit","Главный вход","108H_exit"}},
                 {"108H", {"108H_exit"}}, {"108H_exit", {"108H" ,"staircase11","106H_exit","Главный вход","109H_exit"}},
                 {"109H", {"109H_exit"}}, {"109H_exit", {"109H" ,"staircase11","110H_exit","108H_exit"}},
                 {"110H", {"110H_exit"}}, {"110H_exit", {"110H","109H_exit","111H_exit","112H_exit"}},
                 {"111H", {"111H_exit"}}, {"111H_exit", {"111H","110H_exit"}},
                 {"112H", {"112H_exit"}}, {"112H_exit", {"112H","110H_exit","113H_exit"}},
                 {"113H", {"113H_exit"}}, {"113H_exit", {"113H","staircase21","112H_exit","114H_exit"}},
                 {"114H", {"114H_exit"}}, {"114H_exit", {"114H","staircase21","113H_exit","115H_exit"}},
                 {"115H", {"115H_exit"}}, {"115H_exit", {"115H","114H_exit"}},

                 {"201H", {"201H_exit"}}, {"201H_exit", {"201H", "202H_exit"}},
                 {"202H", {"202H_exit"}}, {"202H_exit", {"202H", "201H_exit","203H_exit"}},
                 {"203H", {"203H_exit"}}, {"203H_exit", {"203H", "staircase2","202H_exit","205H_exit"}},
                 {"205H", {"205H_exit"}}, {"205H_exit", {"205H", "staircase2","206H_exit","203H_exit"}},
                 {"206H", {"206H_exit"}}, {"206H_exit", {"206H","205H_exit","208H_exit","207H_exit"}},
                 {"207H", {"207H_exit"}}, {"207H_exit", {"207H","206H_exit","208H_exit"}},
                 {"208H", {"208H_exit"}}, {"208H_exit", {"208H","staircase12","207H_exit","209H_exit"}},
                 {"209H", {"209H_exit"}}, {"209H_exit", {"209H","staircase12","210H_exit","208H_exit"}},
                 {"210H", {"210H_exit"}}, {"210H_exit", {"210H","209H_exit","211H_exit","212H_exit"}},
                 {"211H", {"211H_exit"}}, {"211H_exit", {"211H","210H_exit"}},
                 {"212H", {"212H_exit"}}, {"212H_exit", {"212H","210H_exit"}},
                 {"213H", {"213H_exit"}}, {"213H_exit", {"213H","staircase22","214H_exit"}},
                 {"214H", {"214H_exit"}}, {"214H_exit", {"214H","staircase22","213H_exit","215H_exit"}},
                 {"215H", {"215H_exit"}}, {"215H_exit", {"215H","214H_exit","216H_exit"}},
                 {"216H", {"216H_exit"}}, {"216H_exit", {"216H","215H_exit"}},

                 {"301H", {"301H_exit"}}, {"301H_exit", {"301H", "302H_exit"}},
                 {"302H", {"302H_exit"}}, {"302H_exit", {"302H", "301H_exit","303H_exit"}},
                 {"303H", {"303H_exit"}}, {"303H_exit", {"303H", "staircase3","302H_exit","305H_exit"}},
                 {"304H", {"304H_exit"}}, {"304H_exit", {"304H", "staircase3"}},
                 {"305H", {"305H_exit"}}, {"305H_exit", {"305H", "staircase3","306H_exit","303H_exit"}},
                 {"306H", {"306H_exit"}}, {"306H_exit", {"306H","305H_exit","308H_exit"}},
                 {"307H", {"307H_exit"}}, {"307H_exit", {"307H","306H_exit","308H_exit"}},
                 {"308H", {"308H_exit"}}, {"308H_exit", {"308H","staircase13","306H_exit","309H_exit"}},
                 {"309H", {"309H_exit"}}, {"309H_exit", {"309H","staircase13","310H_exit","308H_exit"}},
                 {"310H", {"310H_exit"}}, {"310H_exit", {"310H","309H_exit","311H_exit","312H_exit"}},
                 {"311H", {"311H_exit"}}, {"311H_exit", {"311H","310H_exit"}},
                 {"312H", {"312H_exit"}}, {"312H_exit", {"312H","310H_exit","313H_exit"}},
                 {"313H", {"313H_exit"}}, {"313H_exit", {"313H","staircase23","312H_exit","314H_exit"}},
                 {"314H", {"314H_exit"}}, {"314H_exit", {"314H","staircase23","313H_exit","315H_exit"}},
                 {"315H", {"315H_exit"}}, {"315H_exit", {"315H","314H_exit"}},


                 {"staircase", {"staircase2", "103H_exit", "105H_exit","104H_exit"}},
                 {"staircase2", {"staircase","staircase3", "205H_exit", "203H_exit"}},
                 {"staircase3", {"staircase2", "303H_exit", "305H_exit","304H_exit"}},

                 {"staircase11", {"staircase12", "108H_exit", "109H_exit"}},
                 {"staircase12", {"staircase11","staircase13", "209H_exit", "208H_exit"}},
                 {"staircase13", {"staircase12", "309H_exit", "308H_exit"}},

                 {"staircase21", {"staircase22", "113H_exit", "114H_exit"}},
                 {"staircase22", {"staircase21","staircase23", "213H_exit", "214H_exit"}},
                 {"staircase23", {"staircase22", "313H_exit", "314H_exit"}},

                 };
    }


    QString currentFloor() const { return m_currentFloor; }
    QString stepByStepRoute() const { return m_stepByStepRoute; }

    void setCurrentFloor(const QString &floor) {
        if (m_currentFloor != floor) {
            m_currentFloor = floor;
            emit currentFloorChanged();
        }
    }

    Q_INVOKABLE QVariantList getRouteForFloor(int floor) const {
        QVariantList filteredRoute;
        const QVariantList* route = nullptr;

        // Выбираем маршруты в зависимости от этажа
        if (floor == 1) {
            route = &m_routeFloor1;
        } else if (floor == 2) {
            route = &m_routeFloor2;
        } else if (floor == 3) {
            route = &m_routeFloor3;
        }

        if (route) {
            // Фильтруем вершины по этажу
            for (const auto &pointData : *route) {
                if (pointData.toMap().value("color").toString() == QString::number(floor)) {
                    filteredRoute.append(pointData);
                }
            }
        }

        return filteredRoute;
    }

    // Обновленная логика с фильтрацией лестниц и маршрута
    Q_INVOKABLE void findRoute(const QString &startRoom, const QString &endRoom) {
        QVariantMap result;
        if (startRoom.isEmpty() || endRoom.isEmpty()) {
            result["error"] = "Пустая начальная или конечная точка";
            emit routeUpdated(result);
            return;
        }

        if (coordinates.find(startRoom) == coordinates.end() || coordinates.find(endRoom) == coordinates.end()) {
            result["error"] = "Кабинет не найден!";
            emit routeUpdated(result);
            return;
        }

        m_routeFloor1.clear();
        m_routeFloor2.clear();
        m_routeFloor3.clear();

        int startFloor = std::get<2>(coordinates[startRoom]);
        int endFloor = std::get<2>(coordinates[endRoom]);

        QStringList steps;

        // Функция для добавления маршрута в список
        auto addToRoute = [&](const std::vector<QString> &route) {
            for (const auto &point : route) {
                int floor = std::get<2>(coordinates[point]);
                QVariantMap pointData{
                    {"x", std::get<0>(coordinates[point])},
                    {"y", std::get<1>(coordinates[point])},
                    {"color", QString::number(floor)}
                };
                if (floor == 1) m_routeFloor1.append(pointData);
                if (floor == 2) m_routeFloor2.append(pointData);
                if (floor == 3) m_routeFloor3.append(pointData);
            }
        };

        if (startFloor == endFloor) {
            // Один этаж
            auto route = dijkstra(startRoom, endRoom);
            if (!route.empty()) {
                steps.append("Маршрут по этажу " + QString::number(startFloor));
                addToRoute(route);
                steps.append(startRoom + " -> " + endRoom);
            } else {
                result["error"] = "Маршрут не найден на этом этаже!";
                emit routeUpdated(result);
                return;
            }
        } else {
            // Разные этажи
            QVector<QPair<QString, QString>> stairs;

            // Составляем список лестниц для перехода между этажами
            if (startFloor == 1) {
                stairs = {
                    {"staircase", "staircase2"},
                    {"staircase", "staircase3"},
                    {"staircase11", "staircase12"},
                    {"staircase11", "staircase13"},
                    {"staircase21", "staircase22"},
                    {"staircase21", "staircase23"}
                };
            } else if (startFloor == 2) {
                stairs = {
                    {"staircase2", "staircase"},
                    {"staircase2", "staircase3"},
                    {"staircase12", "staircase11"},
                    {"staircase12", "staircase13"},
                    {"staircase22", "staircase21"},
                    {"staircase22", "staircase23"}
                };
            } else if (startFloor == 3) {
                stairs = {
                    {"staircase3", "staircase2"},
                    {"staircase3", "staircase"},
                    {"staircase13", "staircase12"},
                    {"staircase13", "staircase11"},
                    {"staircase23", "staircase22"},
                    {"staircase23", "staircase21"}
                };
            }

            int minDistance = INT_MAX;
            std::vector<QString> bestRoute1, bestRoute2;

            // Проверяем маршруты через все доступные лестницы
            for (const auto &pair : stairs) {
                auto path1 = dijkstra(startRoom, pair.first);
                auto path2 = dijkstra(pair.second, endRoom);
                if (!path1.empty() && !path2.empty()) {
                    int totalLength = path1.size() + path2.size();
                    if (totalLength < minDistance) {
                        minDistance = totalLength;
                        bestRoute1 = path1;
                        bestRoute2 = path2;
                    }
                }
            }

            if (bestRoute1.empty() || bestRoute2.empty()) {
                result["error"] = "Нет маршрута через лестницу!";
                emit routeUpdated(result);
                return;
            }

            steps.append("Маршрут с переходом на этаж " + QString::number(endFloor));
            steps.append(startRoom + " -> лестница");
            addToRoute(bestRoute1);

            steps.append("Переход на этаж " + QString::number(endFloor));
            addToRoute(bestRoute2);
            steps.append("Лестница -> " + endRoom);
        }


        m_stepByStepRoute = steps.join("\n");
        emit stepByStepRouteChanged();

        result["floor1"] = m_routeFloor1;
        result["floor2"] = m_routeFloor2;
        result["floor3"] = m_routeFloor3;
        emit routeUpdated(result);
    }







signals:
    void currentFloorChanged();
    void routeUpdated(const QVariantMap &routeData);
    void stepByStepRouteChanged();

private:
    QString m_currentFloor;
    QString m_stepByStepRoute;

    QVariantList m_routeFloor1;
    QVariantList m_routeFloor2;
    QVariantList m_routeFloor3;

    std::map<QString, std::tuple<int, int, int>> coordinates;
    std::map<QString, std::vector<QString>> graph;

    int distance(const std::tuple<int, int, int> &a, const std::tuple<int, int, int> &b) {
        return static_cast<int>(std::sqrt(std::pow(std::get<0>(a) - std::get<0>(b), 2) +
                                          std::pow(std::get<1>(a) - std::get<1>(b), 2)));
    }

    std::vector<QString> dijkstra(const QString &start, const QString &end) {
        std::map<QString, int> dist;
        std::map<QString, QString> prev;
        std::priority_queue<std::pair<int, QString>, std::vector<std::pair<int, QString>>, std::greater<>> pq;

        for (const auto &node : graph) {
            dist[node.first] = std::numeric_limits<int>::max();
        }
        dist[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            QString current = pq.top().second;
            pq.pop();
            if (current == end) break;

            for (const auto &neighbor : graph[current]) {
                int newDist = dist[current] + distance(coordinates[current], coordinates[neighbor]);
                if (newDist < dist[neighbor]) {
                    dist[neighbor] = newDist;
                    prev[neighbor] = current;
                    pq.push({newDist, neighbor});
                }
            }
        }

        std::vector<QString> path;
        for (QString at = end; !at.isEmpty(); at = prev[at]) {
            path.push_back(at);
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    Navigator navigator;
    engine.rootContext()->setContextProperty("navigator", &navigator);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    engine.loadFromModule("plov", "Main");
    if (engine.rootObjects().isEmpty()) return -1;
    return app.exec();
}
