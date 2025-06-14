"use strict";
var __defProp = Object.defineProperty;
var __getOwnPropDesc = Object.getOwnPropertyDescriptor;
var __getOwnPropNames = Object.getOwnPropertyNames;
var __hasOwnProp = Object.prototype.hasOwnProperty;
var __export = (target, all) => {
  for (var name in all)
    __defProp(target, name, { get: all[name], enumerable: true });
};
var __copyProps = (to, from, except, desc) => {
  if (from && typeof from === "object" || typeof from === "function") {
    for (let key of __getOwnPropNames(from))
      if (!__hasOwnProp.call(to, key) && key !== except)
        __defProp(to, key, { get: () => from[key], enumerable: !(desc = __getOwnPropDesc(from, key)) || desc.enumerable });
  }
  return to;
};
var __toCommonJS = (mod) => __copyProps(__defProp({}, "__esModule", { value: true }), mod);

// ../extension-boilerplate/src/index.tsx
var index_exports = {};
__export(index_exports, {
  default: () => index_default
});
module.exports = __toCommonJS(index_exports);
var import_api = require("@omnicast/api");
var import_jsx_runtime = require("react/jsx-runtime");
var fruits = [
  { emoji: "\u{1F34E}", name: "Apple" },
  { emoji: "\u{1F34A}", name: "Orange" },
  { emoji: "\u{1F34C}", name: "Banana" },
  { emoji: "\u{1F349}", name: "Watermelon" },
  { emoji: "\u{1F347}", name: "Grapes" },
  { emoji: "\u{1F353}", name: "Strawberry" },
  { emoji: "\u{1F34D}", name: "Pineapple" },
  { emoji: "\u{1F96D}", name: "Mango" },
  { emoji: "\u{1F351}", name: "Peach" },
  { emoji: "\u{1F350}", name: "Pear" },
  { emoji: "\u{1F95D}", name: "Kiwi" },
  { emoji: "\u{1F352}", name: "Cherries" },
  { emoji: "\u{1FAD0}", name: "Blueberries" },
  { emoji: "\u{1F965}", name: "Coconut" },
  { emoji: "\u{1F34B}", name: "Lemon" },
  { emoji: "\u{1F348}", name: "Melon" },
  { emoji: "\u{1F34F}", name: "Green Apple" },
  { emoji: "\u{1F951}", name: "Avocado" },
  { emoji: "\u{1FAD2}", name: "Olive" },
  { emoji: "\u{1F345}", name: "Tomato" }
];
var FruitList = () => {
  const handleCustomCallback = (fruit) => {
    console.log("custom callback fired with", fruit);
  };
  console.log("yolo+1");
  return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_api.List, { children: fruits.map((fruit) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)(
    import_api.List.Item,
    {
      title: fruit.name,
      icon: fruit.emoji,
      actions: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_api.ActionPanel, { children: [
        /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_api.Action.CopyToClipboard, { content: "Copy emoji" }),
        /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_api.Action, { title: "Custom callback", onAction: () => handleCustomCallback(fruit) })
      ] })
    },
    fruit.name
  )) });
};
var ExampleCommand = () => {
  return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(FruitList, {});
};
var index_default = ExampleCommand;
