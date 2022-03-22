const fs = require('fs');

const args = process.argv.slice(2);
if (args.length < 2) {
    console.log("You need to provide two arguments: the input file name and the output file name.")
    process.exit(1)
}

const pixelWidth = 48, pixelHeight = 48;

const levelToScene2 = (fileName) => {
    let jsonData = fs.readFileSync(fileName, 'utf-8');
    let data = JSON.parse(jsonData);
    const { room, width, height, layout, enemies } = data;

    return {
        "comments": `This is the root node for the world scene for room ${room}.`,
        "type": "Node",
        "format": {
            "type": "Anchored"
        },
        "layout": {
            "x_anchor": "left",
            "y_anchor": "bottom"
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
                    "anchor": [0, 0],
                    "size": [pixelWidth * width, pixelHeight * height],
                },
                "layout" : {
                    "x_anchor" : "bottom",
                    "y_anchor" : "left"
                },
                "children": Object.assign({}, ...layout.reduce((tile, type, index) => {
                    let row = Math.floor(index / width);
                    let col = index % width;
                    let obj = {};
                    obj[`tile-(${col}-${height - row})`] = {
                        "type": "Node",
                        "format": {
                            "type": "Anchored"
                        },
                        "layout": {
                            "x_index": col,
                            "y_index": height - row,
                            "x_anchor": "left",
                            "y_anchor": "bottom",
                        },
                        "children": {
                            "tile": {
                                "type": "Widget",
                                "data": {
                                    "key": `tile-${type}`,
                                    "variables": getVariables(type, row)
                                }
                            }
                        }
                    }

                    tile.push(obj);
                    return tile;
                }, []))
            },
            "enemies": {
                "type": "Node",
                "format": {
                    "type": "Grid",
                    "width": width,
                    "height": height
                },
                "data": {
                    "anchor": [0, 0],
                    "size": [pixelWidth * width, pixelHeight * height],
                },
                "layout" : {
                    "x_anchor" : "bottom",
                    "y_anchor" : "left"
                },
                "children": Object.assign({}, ...enemies.reduce((enemy, info, index) => {
                    let obj = {};
                    obj[`enemy-(${info["position"][0]}-${height-info["position"][1]})-${index}`] = {
                        "type": info["type"],
                        "layout": {
                            "x_index": info["position"][0],
                            "y_index": height - info["position"][1],
                            "x_anchor": "left",
                            "y_anchor": "bottom",
                        }
                    }
                    enemy.push(obj);
                    return enemy;
                }, []))
            }
        }
        
    }
}

const getVariables = (type, row) => {
    // Will be able to enumerate all the specific variables for each tile type.
    // This includes putting decorations on tiles, or setting the floor type
    // for other tiles.
    switch (type) {
        case 1: // Floor
            return { };
        default:
            return ({
                "priority": row
            });
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
