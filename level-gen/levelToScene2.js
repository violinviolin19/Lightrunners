const fs = require('fs');

const args = process.argv.slice(2);
if (args.length < 2) {
    console.log("You need to provide two arguments: the input file name and the output file name.")
    process.exit(1)
}

const tilesIdsThatShowFloor = [2, 3, 4, 8, 9, 10, 11, 12];
const floorTileId = 0;

const levelToScene2 = (fileName) => {
    let jsonData = fs.readFileSync(fileName, 'utf-8');
    let data = JSON.parse(jsonData);
    const { level, width, height, layout } = data;
    const backgroundColor = "#000000";
    const pixelWidth = 64, pixelHeight = 64;
    const tileScale = 0.4;
    
    return {
        "variables": {},
        "contents": {
            "comments": `This is the root node for the world scene for level ${level}.`,
            "type": "Order",
            "data": {
                "order": "ascend",
            },
            "format": {
                "type": "Anchored"
            },
            "layout": {
                "x_anchor": "center",
                "y_anchor": "middle"
            },
            "children": {
                "tiles": {
                    "type": "Node",
                    "format": {
                        "type": "Grid",
                        "width": width,
                        "height": height
                    },
                    "data": {
                        "anchor": [0.5, 0.5],
                        "size": [pixelWidth * width * tileScale, pixelHeight * height * tileScale],
                    },
                    "children": Object.assign({}, ...layout.reduce((acc, tileTypeId, index) => {
                        let row = Math.floor(index / width);
                        let col = index % width;
                        let obj = {};
                        let tileObj = {};
                        tileObj = {
                            "type": "Image",
                            "format": {
                                "type": "Anchored"
                            },
                            "data": {
                                "texture": `floor-tile-${tileTypeId}`,
                                "anchor": [0.5, 0.5],
                                "scale": tileScale,
                                "priority": row,
                                // "size": [80, 64]
                            },
                            "layout": {
                                "x_index": col,
                                "y_index": height - row,
                                "x_anchor": "left",
                                "y_anchor": "bottom"
                            }
                        }
                        obj[`tile-(${row}-${col})`] = tileObj;
                        acc.push(obj);

                        if (tilesIdsThatShowFloor.includes(tileTypeId)) {
                            let floorObj = {};
                            let floorTileObj = {};

                            floorTileObj = {
                                "type": "Image",
                                "format": {
                                    "type": "Anchored"
                                },
                                "data": {
                                    "texture": `floor-tile-${floorTileId}`,
                                    "anchor": [0.5, 0.5],
                                    "scale": tileScale,
                                    "priority": 0,
                                },
                                "layout": {
                                    "x_index": col,
                                    "y_index": height - row,
                                    "x_anchor": "left",
                                    "y_anchor": "bottom"
                                }
                            }
                            floorObj[`tile-(${row}-${col})-floor`] = floorTileObj;
                            acc.push(floorObj);
                        }
                        return acc;
                    }, [])),
                    "layout" : {
                        "x_anchor" : "center",
                        "y_anchor" : "middle"
                    }
                }
            }
        }
    }
}

const saveToFile = (fileName, scene2Obj) => {
    const data = JSON.stringify(scene2Obj, null, 2);

    fs.writeFile(fileName, data, (err) => {
        if (err) { console.log(err); }
        console.log("JSON data saved.");
    });
}

const scene2Obj = levelToScene2(args[0]);
saveToFile(args[1], scene2Obj);
