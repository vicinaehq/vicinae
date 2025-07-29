"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const jsx_runtime_1 = require("react/jsx-runtime");
const api_1 = require("@omnicast/api");
// Create an array of fruits with their emoji and names
const fruits = [
    { emoji: "🍎", name: "Apple" },
    { emoji: "🍊", name: "Orange" },
    { emoji: "🍌", name: "Banana" },
    { emoji: "🍉", name: "Watermelon" },
    { emoji: "🍇", name: "Grapes" },
    { emoji: "🍓", name: "Strawberry" },
    { emoji: "🍍", name: "Pineapple" },
    { emoji: "🥭", name: "Mango" },
    { emoji: "🍑", name: "Peach" },
    { emoji: "🍐", name: "Pear" },
    { emoji: "🥝", name: "Kiwi" },
    { emoji: "🍒", name: "Cherries" },
    { emoji: "🫐", name: "Blueberries" },
    { emoji: "🥥", name: "Coconut" },
    { emoji: "🍋", name: "Lemon" },
    { emoji: "🍈", name: "Melon" },
    { emoji: "🍏", name: "Green Apple" },
    { emoji: "🥑", name: "Avocado" },
    { emoji: "🫒", name: "Olive" },
    { emoji: "🍅", name: "Tomato" }
];
const FruitList = () => {
    const handleCustomCallback = (fruit) => {
        console.log('custom callback fired with', fruit);
    };
    return ((0, jsx_runtime_1.jsx)(api_1.List, { children: fruits.map(fruit => ((0, jsx_runtime_1.jsx)(api_1.List.Item, { title: fruit.name, icon: fruit.emoji, actions: (0, jsx_runtime_1.jsxs)(api_1.ActionPanel, { children: [(0, jsx_runtime_1.jsx)(api_1.Action.CopyToClipboard, { title: "Copy to clipboard", content: fruit.emoji }), (0, jsx_runtime_1.jsx)(api_1.Action, { title: "Custom callback", icon: api_1.Icon.Pencil, onAction: () => handleCustomCallback(fruit) })] }) }, fruit.name))) }));
};
const ExampleCommand = () => {
    return (0, jsx_runtime_1.jsx)(FruitList, {});
};
exports.default = ExampleCommand;
