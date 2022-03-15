const fs = require('fs');

const args = process.argv.slice(2);
if (args.length < 2) {
    console.log("You need to provide two arguments: the input file name and the output file name.")
    process.exit(1)
}

const floorTileId = 0;
const wallTileId = 1;
const enemyTileId = 7;
const halfWallTileId = 10;
const wallFaceTileId = 11;

const tileIdsThatShowFloor = [2, 3, 4, 7, 8, 9];

const tileIdToType = {
    "0": "BasicTile",
    "1": "Wall",
    "2": "Wall",
    "3": "Wall",
    "4": "Wall",
    "5": "BasicTile",
    "6": "BasicTile",
    "7": "BasicTile",
    "8": "BasicTile",
    "9": "BasicTile",
    "10": "Wall",
    "11": "Wall",
    "12": "Door"
}

const pixelWidth = 64, pixelHeight = 64;
const tileScale = 0.6;

const levelToScene2 = (fileName) => {
    let jsonData = fs.readFileSync(fileName, 'utf-8');
    let data = JSON.parse(jsonData);
    const { level, width, height, layout } = data;
    const layoutWithSpecialWallTiles = convertWallTiles(layout, width, height);
    
    return {
        "comments": `This is the root node for the world scene for level ${level}.`,
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
                    "size": [pixelWidth * width * tileScale, pixelHeight * height * tileScale],
                },
                "layout" : {
                    "x_anchor" : "bottom",
                    "y_anchor" : "left"
                },
                "children": Object.assign({}, ...layoutWithSpecialWallTiles.reduce((acc, tileTypeId, index) => {
                    let row = Math.floor(index / width);
                    let col = index % width;

                    if (tileIdsThatShowFloor.includes(tileTypeId)) {
                        let obj = {};
                        let floorAnchorNode = getTileNode(col, row, width, height, floorTileId, 0);
                        obj[`tile-(${row}-${col})-floor`] = floorAnchorNode;
                        acc.push(obj);

                        // Enemies are added to the "enemies" node, just need to add floor
                        if (tileTypeId === enemyTileId || tileTypeId === floorTileId) {
                            return acc;
                        }
                    }

                    let anchorNode = getTileNode(col, row, width, height, tileTypeId, row);
                    if (tileTypeId === wallTileId) {
                        anchorNode["children"]["tile"]["layout"]["y_offset"] = 46 * tileScale;
                    }
                    if (tileTypeId === halfWallTileId) {
                        anchorNode["children"]["tile"]["layout"]["y_offset"] = 46 * tileScale;
                    }
                    if (tileTypeId === wallFaceTileId) {
                        let obj = {};
                        const wallAnchorNode = getTileNode(col, row, width, height, wallTileId, row);
                        wallAnchorNode["children"]["tile"]["layout"]["y_offset"] = 46 * tileScale;
                        obj[`tile-(${row}-${col})-floor`] = wallAnchorNode;
                        acc.push(obj);
                    }

                    let obj = {};
                    obj[`tile-(${row}-${col})`] = anchorNode;
                    acc.push(obj);
                    return acc;
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
                    "size": [pixelWidth * width * tileScale, pixelHeight * height * tileScale],
                },
                "layout" : {
                    "x_anchor" : "bottom",
                    "y_anchor" : "left"
                },
                "children": Object.assign({}, ...layoutWithSpecialWallTiles.reduce((acc, tileTypeId, index) => {
                    let row = Math.floor(index / width);
                    let col = index % width;

                    if (tileTypeId !== enemyTileId) {
                        return acc;
                    }

                    // TODO: Make it so this doesn't render
                    let anchorNode = getTileNode(col, row, width, height, floorTileId, 0);
                    let obj = {};
                    obj[`enemy-(${row}-${col})`] = anchorNode;
                    acc.push(obj);
                    return acc;
                }, []))
            }
        }
        
    }
}

const getTileNode = (col, row, width, height, tileTypeId, priority) => {
    if (tileTypeId === floorTileId) {
        priority = 0;
    }
    let anchorNode = {
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
        "children": {}
    }

    let tileObj = {
        "type": tileIdToType[tileTypeId],
        "format": {
            "type": "Anchored"
        },
        "data": {
            "texture": `floor-tile-${tileTypeId}`,
            "anchor": [0, 0],
            "scale": tileScale,
            "priority": priority,
        },
        "layout": {
            "x_anchor": "left",
            "y_anchor": "bottom",
            "absolute": true
        }
    }

    anchorNode["children"]["tile"] = tileObj;
    return anchorNode;
}

const convertWallTiles = (arr, width, height) => {
    const finalArr = [...arr];
    for (let i = 0; i < height; i++) {   // Rows
        for (let j = 0; j < width; j++) {   // Columns
            if (arr[i * width + j] === wallTileId) {
                // Wall tiles with non-wall below them become wall-faces (facing down)
                if (i < height - 1 && arr[(i + 1) * width + j] !== wallTileId) {
                    finalArr[i * width + j] = wallFaceTileId;
                }
                // Wall tiles with non-wall above them become half-walls
                else if (i > 0 && arr[(i - 1) * width + j] !== wallTileId) {
                    finalArr[i * width + j] = halfWallTileId;
                }
            }
        }
    }
    return finalArr;
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
