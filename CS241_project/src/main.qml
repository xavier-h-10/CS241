import QtQuick 2.15
import QtQuick.Window 2.15
import QtLocation 5.15
import QtPositioning 5.15

Map {
    width: Qt.platform.os == "android" ? Screen.width : 512
 //   height: Qt.platform.os == "android" ? Screen.height : 512
    visible: true

    Plugin {
        id: mapPlugin
        name: "osm" // "mapboxgl", "esri", ...
        // specify plugin parameters if necessary
        // PluginParameter {
        //     name:
        //     value:
        // }
    }
    property variant locationChengdu: QtPositioning.coordinate(30.66, 104.065) // 先纬度后经度

    Map {
        anchors.fill: parent
        plugin: mapPlugin
        center: locationChengdu
        zoomLevel: 14
        objectName: "Map"

    /*    MapItemView {
               model: searchModel
               delegate: MapQuickItem {
                   coordinate: place.location.coordinate

                   anchorPoint.x: image.width * 0.5
                   anchorPoint.y: image.height

                   sourceItem: Column {
                       Image { id: image; source: "marker.png" }
                       Text { text: title; font.bold: true }
                   }
               }
        }*/

        MapQuickItem{
            id: point_1
            objectName: "point_1"
            //缩放等级默认0固定大小，否则会和缩放等级一起放大缩小
            zoomLevel: 0
            //指示的坐标点
            coordinate: QtPositioning.coordinate(30.66, 104.065)
            //sourceItem左上角相对于coordinate的偏移
            anchorPoint: Qt.point(sourceItem.width/2,sourceItem.height)
            sourceItem: Column {
                Image { id: image; source: "marker.png" }
                Text { text: "Pick-up Location"; font.bold: true }
            /*    MouseArea
                      {
                          anchors.fill: parent
                          drag.target: point_1
                          onClicked: {
                              console.log("map item coor", point_1.coordinate.latitude, point_1.coordinate.longitude)
                          }
                      }*/
            }
        }



        MapQuickItem{
            id: point_2
            //缩放等级默认0固定大小，否则会和缩放等级一起放大缩小
            zoomLevel: 0
            //指示的坐标点
            coordinate: QtPositioning.coordinate(0,0)
            //sourceItem左上角相对于coordinate的偏移
            anchorPoint: Qt.point(sourceItem.width/2,sourceItem.height)
            sourceItem: Column {
                Image { id: image_1; source: "marker.png" }
                Text { text: "Drop-off Location"; font.bold: true }
            }
        }

        function update_point1(x,y)
        {
            console.log("update_point1 called");
            point_1.coordinate=QtPositioning.coordinate(x,y)
        }

        function update_point2(x,y)
        {
            console.log("update_point2 called");
            point_2.coordinate=QtPositioning.coordinate(x,y)
        }
    /*    GeocodeModel {
            plugin: mapPlugin
            query :"Chengdu"
            onLocationsChanged:
                if(count)
                {
                    point_1.coordinate=get(0).coordinate;
                }
            Component.onCompleted: update()
        }*/

    }

}




