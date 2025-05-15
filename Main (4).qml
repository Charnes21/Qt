import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Shapes

Window {
    id: root
    width: 800
    height: 600
    visible: true
    title: qsTr("Навигатор")

    property string currentImageSource: "qrc:/images2/cropped.svg"
    property string currentFloor: "1fl" // Какой этаж
    property string startCabinet: "" // Сохраненная начальная точка
    property string endCabinet: "" // Сохраненная конечная точка
    property var savedRoute: [] // Сохраненные точки маршрута
    property var routeDataByFloor: {
        "1fl": [],
        "2fl": [],
        "3fl": []
    }

    // Словарь с координатами кабинетов для разных этажей
    property var coordinates: {
        "1fl": {
            "101H": {x: 400, y: 1600}, "102H": {x: 580, y: 1680}, "103H": {x: 700, y: 1680},
            "104H": {x: 770, y: 1485},"105H": {x: 1025, y: 1700},"106H": {x: 1300, y: 1700},
            "108H": {x: 1800, y: 1600},"109H": {x: 1830, y: 1100},"110H": {x: 1830, y: 850},
            "111H": {x: 1500, y: 700},"112H": {x: 1630, y: 250},"113H": {x: 760, y: 250},"114H": {x: 450, y: 250},"115H": {x: 80, y: 350}
        },
        "2fl": {
            "201H": {x: 400, y: 1600}, "202H": {x: 565, y: 1680}, "203H": {x: 670, y: 1680},
            "210H": {x: 1000, y: 400},"205H": {x: 1025, y: 1700},"206H": {x: 1300, y: 1700},
            "207H": {x: 1550, y: 1800},"208H": {x: 1800, y: 1600},"209H": {x: 1830, y: 1100},
            "210H": {x: 1830, y: 850},"211H": {x: 1500, y: 700},"212H": {x: 1630, y: 250},"213H": {x: 760, y: 250},"214H": {x: 450, y: 250},"215H": {x: 260, y: 250},"216H": {x: 80, y: 350}
        },
        "3fl": {
            "301H": {x: 400, y: 1600}, "303H": {x: 690, y: 1680},"307H": {x: 1550, y: 1800},
            "302H": {x: 570, y: 1680},"304H": {x: 760, y: 1485},"305H": {x: 1025, y: 1700},
            "306H": {x: 1300, y: 1700},"308H": {x: 1800, y: 1600},"309H": {x: 1830, y: 1100},
            "310H": {x: 1830, y: 850},"311H": {x: 1500, y: 700},"312H": {x: 1630, y: 250},"313H": {x: 760, y: 250},"314H": {x: 450, y: 250},"315H": {x: 80, y: 350}
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "white"

        Flickable {
            id: flickable
            anchors.fill: parent
            contentWidth: map.width
            contentHeight: map.height
            clip: true

            PinchArea {
                id: pinchArea
                anchors.fill: parent

                property real scaleFactor: 0.2
                property real minScale: 0.2
                property real maxScale: 1.5
                property real pinchSpeed: 0.1

                onPinchUpdated: {
                    var scaleChange = pinch.scale;
                    scaleFactor *= (scaleChange - 1) * pinchSpeed + 1;
                    if (scaleFactor < minScale) scaleFactor = minScale;
                    if (scaleFactor > maxScale) scaleFactor = maxScale;

                    var newWidth = map.originalWidth * scaleFactor;
                    var newHeight = map.originalHeight * scaleFactor;

                    flickable.resizeContent(newWidth, newHeight, pinch.center);

                    map.width = newWidth;
                    map.height = newHeight;

                    flickable.returnToBounds();
                    routeCanvas.requestPaint();
                }

                Image {
                    id: map
                    source: root.currentImageSource
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    property real originalWidth: 2000
                    property real originalHeight: 2000
                    width: originalWidth * pinchArea.scaleFactor
                    height: originalHeight * pinchArea.scaleFactor
                }

                Canvas {
                    id: routeCanvas
                    anchors.fill: parent
                    property var routePoints: []
                    property string currentFloor: root.currentFloor
                    property var vertices: {}

                    onPaint: {
                        var ctx = routeCanvas.getContext("2d");
                        ctx.clearRect(0, 0, routeCanvas.width, routeCanvas.height);

                        // Отображаем подписанные кабинеты для текущего этажа
                        var floorVertices = root.coordinates[root.currentFloor];
                        for (var key in floorVertices) {
                            var point = floorVertices[key];
                            ctx.font = "14px Arial";
                            ctx.fillStyle = "black";
                            ctx.fillText(key, point.x * pinchArea.scaleFactor, point.y * pinchArea.scaleFactor);
                        }

                        // Отрисовываем маршрут
                        if (routePoints.length > 0) {
                            ctx.beginPath();
                            ctx.strokeStyle = "red";
                            ctx.lineWidth = 3;

                            ctx.moveTo(routePoints[0].x * pinchArea.scaleFactor, routePoints[0].y * pinchArea.scaleFactor);
                            for (var i = 1; i < routePoints.length; i++) {
                                ctx.lineTo(routePoints[i].x * pinchArea.scaleFactor, routePoints[i].y * pinchArea.scaleFactor);
                            }

                            ctx.stroke();
                        }
                    }

                    function drawRoute(points) {
                        routePoints = points;
                        routeCanvas.requestPaint();
                    }

                    function updateRouteForFloor(floor) {
                        var path = navigator.findRoute(startCabinet, endCabinet, floor);
                        if (path.error) {
                            console.log("Ошибка маршрута:", path.error);
                        } else {
                            routeCanvas.drawRoute(path.route);
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        console.log("Координаты: ", mouse.x, mouse.y);
                    }
                }
            }
        }

        // Кнопки для выбора этажа (по центру слева)
        Column {
            spacing: 20
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 20

            Button {
                text: "1"
                onClicked: {
                    root.currentImageSource = "qrc:/images2/cropped.svg";
                    root.currentFloor = "1fl";
                    root.updateCabinetLabels("1fl");
                    routeCanvas.updateRouteForFloor("1fl");
                    if (startCabinet !== "" && endCabinet !== "")
                        navigator.findRoute(startCabinet, endCabinet);
                }
            }

            Button {
                text: "2"
                onClicked: {
                    root.currentImageSource = "qrc:/images2/cropped1.svg";
                    root.currentFloor = "2fl";
                    root.updateCabinetLabels("2fl");
                    routeCanvas.updateRouteForFloor("2fl");
                    if (startCabinet !== "" && endCabinet !== "")
                        navigator.findRoute(startCabinet, endCabinet);
                }
            }

            Button {
                text: "3"
                onClicked: {
                    root.currentImageSource = "qrc:/images2/cropped3.svg";
                    root.currentFloor = "3fl";
                    root.updateCabinetLabels("3fl");
                    routeCanvas.updateRouteForFloor("3fl");
                    if (startCabinet !== "" && endCabinet !== "")
                        navigator.findRoute(startCabinet, endCabinet);
                }
            }
        }

        // Панель для ввода начальной и конечной точки (внизу по центру)
        Column {
            spacing: 10
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 20

            TextField {
                id: startPoint
                placeholderText: "Начальная точка"
                width: 150
            }

            TextField {
                id: endPoint
                placeholderText: "Конечная точка"
                width: 150
            }

            Button {
                text: "Построить маршрут"
                onClicked: {
                    startCabinet = startPoint.text;
                    endCabinet = endPoint.text;
                    navigator.findRoute(startCabinet, endCabinet);
                }
            }

            Text {
                id: routeText
                text: navigator.stepByStepRoute
                wrapMode: Text.Wrap
                width: 200
                horizontalAlignment: Text.AlignHCenter
            }
        }

    }

    // Функция обновления подписей кабинетов
    function updateCabinetLabels(floor) {
        var floorVertices = root.coordinates[floor];
        for (var key in floorVertices) {
            console.log("Подпись кабинета: " + key + " на этаже " + floor);
        }
    }

    // Подключение сигнала от C++ к QML
    Connections {
        target: navigator

        onStepByStepRouteChanged: {
            routeText.text = navigator.stepByStepRoute.join("\n");
        }

        onRouteUpdated: (routeData) => {
            routeDataByFloor["1fl"] = routeData.floor1 || [];
            routeDataByFloor["2fl"] = routeData.floor2 || [];
            routeDataByFloor["3fl"] = routeData.floor3 || [];

            routeCanvas.drawRoute(routeDataByFloor[root.currentFloor]);
            routeCanvas.requestPaint();
        }
    }
}

