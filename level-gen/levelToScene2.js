const fs = require('fs');

const args = process.argv.slice(2);
if (args.length < 2) {
    console.log("You need to provide two arguments: the input file name and the output file name.")
    process.exit(1)
}

const tileIdsThatShowFloor = [2, 3, 4, 8, 9, 10, 11, 12];

// TODO: Find a better data structure to accomplish this.
const tileIdsThatAreSolid = [1, 2, 3, 4, 7];
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
            "type": "Node",
            "format": {
                "type": "Anchored"
            },
            "children": {
                "tiles": {
                    "type": "Solid",
                    "format": {
                        "type": "Grid",
                        "width": width,
                        "height": height
                    },
                    "data": {
                        "anchor": [0, 0],
                        "size": [pixelWidth * width * tileScale, pixelHeight * height * tileScale],
                        "color": backgroundColor
                    },
                    "children": Object.assign({}, ...layout.map((tileTypeId, index) => {
                        let row = Math.floor(index / width);
                        let col = index % width;
                        let obj = {};
                        let tileObj = {};
                        tileObj = {
                            "type": tileIdsThatAreSolid.includes(tileTypeId) ? 'Wall' : 'BasicTile',
                            "format": {
                                "type": "Anchored"
                            },
                            "data": {
                                "texture": tileIdsThatShowFloor.includes(tileTypeId) ? `floor-tile-${floorTileId}` : `floor-tile-${tileTypeId}`,
                                "anchor": [0.5, 0.5],
                                "scale": tileScale
                            },
                            "layout": {
                                "x_index": row,
                                "y_index": col,
                                "x_anchor": "center",
                                "y_anchor": "middle"
                            }
                        }
                        if (tileIdsThatShowFloor.includes(tileTypeId)) {
                            tileObj["children"] = {}
                            tileObj["children"][`tile-(${row}-${col})-floor`] = {
                                "type": 'BasicTile',
                                "format": {
                                    "type": "Anchored"
                                },
                                "data": {
                                    "texture": `floor-tile-${tileTypeId}`,
                                    "anchor": [0.5, 0.5],
                                    "scale": 1
                                },
                                "layout": {
                                    "x_index": row,
                                    "y_index": col,
                                    "x_anchor": "center",
                                    "y_anchor": "middle"
                                }
                            }
                        }
                        obj[`tile-(${row}-${col})`] = tileObj;
                        return obj;
                    })),
                    "layout" : {
                        "x_anchor" : "bottom",
                        "y_anchor" : "left"
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
