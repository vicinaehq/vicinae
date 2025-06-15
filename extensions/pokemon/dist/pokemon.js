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

// src/pokemon.tsx
var pokemon_exports = {};
__export(pokemon_exports, {
  default: () => pokemon_default
});
module.exports = __toCommonJS(pokemon_exports);
var import_api = require("@omnicast/api");
var import_react = require("react");

// src/pokemons.ts
var pokemons_default = [
  {
    "name": "bulbasaur",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "ivysaur",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/2.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "venusaur",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/3.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "charmander",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/4.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "charmeleon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/5.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "charizard",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/6.png",
    "types": [
      "fire",
      "flying"
    ]
  },
  {
    "name": "squirtle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/7.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "wartortle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/8.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "blastoise",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/9.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "caterpie",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "metapod",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/11.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "butterfree",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/12.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "weedle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/13.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "kakuna",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/14.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "beedrill",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/15.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "pidgey",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/16.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "pidgeotto",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/17.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "pidgeot",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/18.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "rattata",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/19.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "raticate",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/20.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "spearow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/21.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "fearow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/22.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "ekans",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/23.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "arbok",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/24.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "pikachu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/25.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "raichu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/26.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "sandshrew",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/27.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "sandslash",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/28.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "nidoran-f",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/29.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "nidorina",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/30.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "nidoqueen",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/31.png",
    "types": [
      "poison",
      "ground"
    ]
  },
  {
    "name": "nidoran-m",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/32.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "nidorino",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/33.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "nidoking",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/34.png",
    "types": [
      "poison",
      "ground"
    ]
  },
  {
    "name": "clefairy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/35.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "clefable",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/36.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "vulpix",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/37.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "ninetales",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/38.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "jigglypuff",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/39.png",
    "types": [
      "normal",
      "fairy"
    ]
  },
  {
    "name": "wigglytuff",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/40.png",
    "types": [
      "normal",
      "fairy"
    ]
  },
  {
    "name": "zubat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/41.png",
    "types": [
      "poison",
      "flying"
    ]
  },
  {
    "name": "golbat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/42.png",
    "types": [
      "poison",
      "flying"
    ]
  },
  {
    "name": "oddish",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/43.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "gloom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/44.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "vileplume",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/45.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "paras",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/46.png",
    "types": [
      "bug",
      "grass"
    ]
  },
  {
    "name": "parasect",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/47.png",
    "types": [
      "bug",
      "grass"
    ]
  },
  {
    "name": "venonat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/48.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "venomoth",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/49.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "diglett",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/50.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "dugtrio",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/51.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "meowth",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/52.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "persian",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/53.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "psyduck",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/54.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "golduck",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/55.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "mankey",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/56.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "primeape",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/57.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "growlithe",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/58.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "arcanine",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/59.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "poliwag",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/60.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "poliwhirl",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/61.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "poliwrath",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/62.png",
    "types": [
      "water",
      "fighting"
    ]
  },
  {
    "name": "abra",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/63.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "kadabra",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/64.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "alakazam",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/65.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "machop",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/66.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "machoke",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/67.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "machamp",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/68.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "bellsprout",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/69.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "weepinbell",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/70.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "victreebel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/71.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "tentacool",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/72.png",
    "types": [
      "water",
      "poison"
    ]
  },
  {
    "name": "tentacruel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/73.png",
    "types": [
      "water",
      "poison"
    ]
  },
  {
    "name": "geodude",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/74.png",
    "types": [
      "rock",
      "ground"
    ]
  },
  {
    "name": "graveler",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/75.png",
    "types": [
      "rock",
      "ground"
    ]
  },
  {
    "name": "golem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/76.png",
    "types": [
      "rock",
      "ground"
    ]
  },
  {
    "name": "ponyta",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/77.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "rapidash",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/78.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "slowpoke",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/79.png",
    "types": [
      "water",
      "psychic"
    ]
  },
  {
    "name": "slowbro",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/80.png",
    "types": [
      "water",
      "psychic"
    ]
  },
  {
    "name": "magnemite",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/81.png",
    "types": [
      "electric",
      "steel"
    ]
  },
  {
    "name": "magneton",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/82.png",
    "types": [
      "electric",
      "steel"
    ]
  },
  {
    "name": "farfetchd",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/83.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "doduo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/84.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "dodrio",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/85.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "seel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/86.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "dewgong",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/87.png",
    "types": [
      "water",
      "ice"
    ]
  },
  {
    "name": "grimer",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/88.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "muk",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/89.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "shellder",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/90.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "cloyster",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/91.png",
    "types": [
      "water",
      "ice"
    ]
  },
  {
    "name": "gastly",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/92.png",
    "types": [
      "ghost",
      "poison"
    ]
  },
  {
    "name": "haunter",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/93.png",
    "types": [
      "ghost",
      "poison"
    ]
  },
  {
    "name": "gengar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/94.png",
    "types": [
      "ghost",
      "poison"
    ]
  },
  {
    "name": "onix",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/95.png",
    "types": [
      "rock",
      "ground"
    ]
  },
  {
    "name": "drowzee",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/96.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "hypno",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/97.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "krabby",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/98.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "kingler",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/99.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "voltorb",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/100.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "electrode",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/101.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "exeggcute",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/102.png",
    "types": [
      "grass",
      "psychic"
    ]
  },
  {
    "name": "exeggutor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/103.png",
    "types": [
      "grass",
      "psychic"
    ]
  },
  {
    "name": "cubone",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/104.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "marowak",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/105.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "hitmonlee",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/106.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "hitmonchan",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/107.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "lickitung",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/108.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "koffing",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/109.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "weezing",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/110.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "rhyhorn",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/111.png",
    "types": [
      "ground",
      "rock"
    ]
  },
  {
    "name": "rhydon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/112.png",
    "types": [
      "ground",
      "rock"
    ]
  },
  {
    "name": "chansey",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/113.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "tangela",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/114.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "kangaskhan",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/115.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "horsea",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/116.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "seadra",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/117.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "goldeen",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/118.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "seaking",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/119.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "staryu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/120.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "starmie",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/121.png",
    "types": [
      "water",
      "psychic"
    ]
  },
  {
    "name": "mr-mime",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/122.png",
    "types": [
      "psychic",
      "fairy"
    ]
  },
  {
    "name": "scyther",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/123.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "jynx",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/124.png",
    "types": [
      "ice",
      "psychic"
    ]
  },
  {
    "name": "electabuzz",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/125.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "magmar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/126.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "pinsir",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/127.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "tauros",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/128.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "magikarp",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/129.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "gyarados",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/130.png",
    "types": [
      "water",
      "flying"
    ]
  },
  {
    "name": "lapras",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/131.png",
    "types": [
      "water",
      "ice"
    ]
  },
  {
    "name": "ditto",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/132.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "eevee",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/133.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "vaporeon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/134.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "jolteon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/135.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "flareon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/136.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "porygon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/137.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "omanyte",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/138.png",
    "types": [
      "rock",
      "water"
    ]
  },
  {
    "name": "omastar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/139.png",
    "types": [
      "rock",
      "water"
    ]
  },
  {
    "name": "kabuto",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/140.png",
    "types": [
      "rock",
      "water"
    ]
  },
  {
    "name": "kabutops",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/141.png",
    "types": [
      "rock",
      "water"
    ]
  },
  {
    "name": "aerodactyl",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/142.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "snorlax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/143.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "articuno",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/144.png",
    "types": [
      "ice",
      "flying"
    ]
  },
  {
    "name": "zapdos",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/145.png",
    "types": [
      "electric",
      "flying"
    ]
  },
  {
    "name": "moltres",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/146.png",
    "types": [
      "fire",
      "flying"
    ]
  },
  {
    "name": "dratini",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/147.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "dragonair",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/148.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "dragonite",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/149.png",
    "types": [
      "dragon",
      "flying"
    ]
  },
  {
    "name": "mewtwo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/150.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "mew",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/151.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "chikorita",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/152.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "bayleef",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/153.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "meganium",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/154.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "cyndaquil",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/155.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "quilava",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/156.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "typhlosion",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/157.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "totodile",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/158.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "croconaw",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/159.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "feraligatr",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/160.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "sentret",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/161.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "furret",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/162.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "hoothoot",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/163.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "noctowl",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/164.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "ledyba",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/165.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "ledian",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/166.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "spinarak",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/167.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "ariados",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/168.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "crobat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/169.png",
    "types": [
      "poison",
      "flying"
    ]
  },
  {
    "name": "chinchou",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/170.png",
    "types": [
      "water",
      "electric"
    ]
  },
  {
    "name": "lanturn",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/171.png",
    "types": [
      "water",
      "electric"
    ]
  },
  {
    "name": "pichu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/172.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "cleffa",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/173.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "igglybuff",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/174.png",
    "types": [
      "normal",
      "fairy"
    ]
  },
  {
    "name": "togepi",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/175.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "togetic",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/176.png",
    "types": [
      "fairy",
      "flying"
    ]
  },
  {
    "name": "natu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/177.png",
    "types": [
      "psychic",
      "flying"
    ]
  },
  {
    "name": "xatu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/178.png",
    "types": [
      "psychic",
      "flying"
    ]
  },
  {
    "name": "mareep",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/179.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "flaaffy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/180.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "ampharos",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/181.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "bellossom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/182.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "marill",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/183.png",
    "types": [
      "water",
      "fairy"
    ]
  },
  {
    "name": "azumarill",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/184.png",
    "types": [
      "water",
      "fairy"
    ]
  },
  {
    "name": "sudowoodo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/185.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "politoed",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/186.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "hoppip",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/187.png",
    "types": [
      "grass",
      "flying"
    ]
  },
  {
    "name": "skiploom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/188.png",
    "types": [
      "grass",
      "flying"
    ]
  },
  {
    "name": "jumpluff",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/189.png",
    "types": [
      "grass",
      "flying"
    ]
  },
  {
    "name": "aipom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/190.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "sunkern",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/191.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "sunflora",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/192.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "yanma",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/193.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "wooper",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/194.png",
    "types": [
      "water",
      "ground"
    ]
  },
  {
    "name": "quagsire",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/195.png",
    "types": [
      "water",
      "ground"
    ]
  },
  {
    "name": "espeon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/196.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "umbreon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/197.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "murkrow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/198.png",
    "types": [
      "dark",
      "flying"
    ]
  },
  {
    "name": "slowking",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/199.png",
    "types": [
      "water",
      "psychic"
    ]
  },
  {
    "name": "misdreavus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/200.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "unown",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/201.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "wobbuffet",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/202.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "girafarig",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/203.png",
    "types": [
      "normal",
      "psychic"
    ]
  },
  {
    "name": "pineco",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/204.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "forretress",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/205.png",
    "types": [
      "bug",
      "steel"
    ]
  },
  {
    "name": "dunsparce",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/206.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "gligar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/207.png",
    "types": [
      "ground",
      "flying"
    ]
  },
  {
    "name": "steelix",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/208.png",
    "types": [
      "steel",
      "ground"
    ]
  },
  {
    "name": "snubbull",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/209.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "granbull",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/210.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "qwilfish",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/211.png",
    "types": [
      "water",
      "poison"
    ]
  },
  {
    "name": "scizor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/212.png",
    "types": [
      "bug",
      "steel"
    ]
  },
  {
    "name": "shuckle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/213.png",
    "types": [
      "bug",
      "rock"
    ]
  },
  {
    "name": "heracross",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/214.png",
    "types": [
      "bug",
      "fighting"
    ]
  },
  {
    "name": "sneasel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/215.png",
    "types": [
      "dark",
      "ice"
    ]
  },
  {
    "name": "teddiursa",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/216.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "ursaring",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/217.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "slugma",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/218.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "magcargo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/219.png",
    "types": [
      "fire",
      "rock"
    ]
  },
  {
    "name": "swinub",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/220.png",
    "types": [
      "ice",
      "ground"
    ]
  },
  {
    "name": "piloswine",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/221.png",
    "types": [
      "ice",
      "ground"
    ]
  },
  {
    "name": "corsola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/222.png",
    "types": [
      "water",
      "rock"
    ]
  },
  {
    "name": "remoraid",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/223.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "octillery",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/224.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "delibird",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/225.png",
    "types": [
      "ice",
      "flying"
    ]
  },
  {
    "name": "mantine",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/226.png",
    "types": [
      "water",
      "flying"
    ]
  },
  {
    "name": "skarmory",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/227.png",
    "types": [
      "steel",
      "flying"
    ]
  },
  {
    "name": "houndour",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/228.png",
    "types": [
      "dark",
      "fire"
    ]
  },
  {
    "name": "houndoom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/229.png",
    "types": [
      "dark",
      "fire"
    ]
  },
  {
    "name": "kingdra",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/230.png",
    "types": [
      "water",
      "dragon"
    ]
  },
  {
    "name": "phanpy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/231.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "donphan",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/232.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "porygon2",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/233.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "stantler",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/234.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "smeargle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/235.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "tyrogue",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/236.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "hitmontop",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/237.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "smoochum",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/238.png",
    "types": [
      "ice",
      "psychic"
    ]
  },
  {
    "name": "elekid",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/239.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "magby",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/240.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "miltank",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/241.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "blissey",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/242.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "raikou",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/243.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "entei",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/244.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "suicune",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/245.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "larvitar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/246.png",
    "types": [
      "rock",
      "ground"
    ]
  },
  {
    "name": "pupitar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/247.png",
    "types": [
      "rock",
      "ground"
    ]
  },
  {
    "name": "tyranitar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/248.png",
    "types": [
      "rock",
      "dark"
    ]
  },
  {
    "name": "lugia",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/249.png",
    "types": [
      "psychic",
      "flying"
    ]
  },
  {
    "name": "ho-oh",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/250.png",
    "types": [
      "fire",
      "flying"
    ]
  },
  {
    "name": "celebi",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/251.png",
    "types": [
      "psychic",
      "grass"
    ]
  },
  {
    "name": "treecko",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/252.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "grovyle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/253.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "sceptile",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/254.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "torchic",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/255.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "combusken",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/256.png",
    "types": [
      "fire",
      "fighting"
    ]
  },
  {
    "name": "blaziken",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/257.png",
    "types": [
      "fire",
      "fighting"
    ]
  },
  {
    "name": "mudkip",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/258.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "marshtomp",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/259.png",
    "types": [
      "water",
      "ground"
    ]
  },
  {
    "name": "swampert",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/260.png",
    "types": [
      "water",
      "ground"
    ]
  },
  {
    "name": "poochyena",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/261.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "mightyena",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/262.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "zigzagoon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/263.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "linoone",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/264.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "wurmple",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/265.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "silcoon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/266.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "beautifly",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/267.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "cascoon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/268.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "dustox",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/269.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "lotad",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/270.png",
    "types": [
      "water",
      "grass"
    ]
  },
  {
    "name": "lombre",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/271.png",
    "types": [
      "water",
      "grass"
    ]
  },
  {
    "name": "ludicolo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/272.png",
    "types": [
      "water",
      "grass"
    ]
  },
  {
    "name": "seedot",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/273.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "nuzleaf",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/274.png",
    "types": [
      "grass",
      "dark"
    ]
  },
  {
    "name": "shiftry",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/275.png",
    "types": [
      "grass",
      "dark"
    ]
  },
  {
    "name": "taillow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/276.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "swellow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/277.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "wingull",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/278.png",
    "types": [
      "water",
      "flying"
    ]
  },
  {
    "name": "pelipper",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/279.png",
    "types": [
      "water",
      "flying"
    ]
  },
  {
    "name": "ralts",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/280.png",
    "types": [
      "psychic",
      "fairy"
    ]
  },
  {
    "name": "kirlia",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/281.png",
    "types": [
      "psychic",
      "fairy"
    ]
  },
  {
    "name": "gardevoir",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/282.png",
    "types": [
      "psychic",
      "fairy"
    ]
  },
  {
    "name": "surskit",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/283.png",
    "types": [
      "bug",
      "water"
    ]
  },
  {
    "name": "masquerain",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/284.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "shroomish",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/285.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "breloom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/286.png",
    "types": [
      "grass",
      "fighting"
    ]
  },
  {
    "name": "slakoth",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/287.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "vigoroth",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/288.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "slaking",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/289.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "nincada",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/290.png",
    "types": [
      "bug",
      "ground"
    ]
  },
  {
    "name": "ninjask",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/291.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "shedinja",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/292.png",
    "types": [
      "bug",
      "ghost"
    ]
  },
  {
    "name": "whismur",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/293.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "loudred",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/294.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "exploud",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/295.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "makuhita",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/296.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "hariyama",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/297.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "azurill",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/298.png",
    "types": [
      "normal",
      "fairy"
    ]
  },
  {
    "name": "nosepass",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/299.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "skitty",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/300.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "delcatty",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/301.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "sableye",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/302.png",
    "types": [
      "dark",
      "ghost"
    ]
  },
  {
    "name": "mawile",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/303.png",
    "types": [
      "steel",
      "fairy"
    ]
  },
  {
    "name": "aron",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/304.png",
    "types": [
      "steel",
      "rock"
    ]
  },
  {
    "name": "lairon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/305.png",
    "types": [
      "steel",
      "rock"
    ]
  },
  {
    "name": "aggron",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/306.png",
    "types": [
      "steel",
      "rock"
    ]
  },
  {
    "name": "meditite",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/307.png",
    "types": [
      "fighting",
      "psychic"
    ]
  },
  {
    "name": "medicham",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/308.png",
    "types": [
      "fighting",
      "psychic"
    ]
  },
  {
    "name": "electrike",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/309.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "manectric",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/310.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "plusle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/311.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "minun",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/312.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "volbeat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/313.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "illumise",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/314.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "roselia",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/315.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "gulpin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/316.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "swalot",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/317.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "carvanha",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/318.png",
    "types": [
      "water",
      "dark"
    ]
  },
  {
    "name": "sharpedo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/319.png",
    "types": [
      "water",
      "dark"
    ]
  },
  {
    "name": "wailmer",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/320.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "wailord",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/321.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "numel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/322.png",
    "types": [
      "fire",
      "ground"
    ]
  },
  {
    "name": "camerupt",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/323.png",
    "types": [
      "fire",
      "ground"
    ]
  },
  {
    "name": "torkoal",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/324.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "spoink",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/325.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "grumpig",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/326.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "spinda",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/327.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "trapinch",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/328.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "vibrava",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/329.png",
    "types": [
      "ground",
      "dragon"
    ]
  },
  {
    "name": "flygon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/330.png",
    "types": [
      "ground",
      "dragon"
    ]
  },
  {
    "name": "cacnea",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/331.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "cacturne",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/332.png",
    "types": [
      "grass",
      "dark"
    ]
  },
  {
    "name": "swablu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/333.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "altaria",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/334.png",
    "types": [
      "dragon",
      "flying"
    ]
  },
  {
    "name": "zangoose",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/335.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "seviper",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/336.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "lunatone",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/337.png",
    "types": [
      "rock",
      "psychic"
    ]
  },
  {
    "name": "solrock",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/338.png",
    "types": [
      "rock",
      "psychic"
    ]
  },
  {
    "name": "barboach",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/339.png",
    "types": [
      "water",
      "ground"
    ]
  },
  {
    "name": "whiscash",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/340.png",
    "types": [
      "water",
      "ground"
    ]
  },
  {
    "name": "corphish",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/341.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "crawdaunt",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/342.png",
    "types": [
      "water",
      "dark"
    ]
  },
  {
    "name": "baltoy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/343.png",
    "types": [
      "ground",
      "psychic"
    ]
  },
  {
    "name": "claydol",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/344.png",
    "types": [
      "ground",
      "psychic"
    ]
  },
  {
    "name": "lileep",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/345.png",
    "types": [
      "rock",
      "grass"
    ]
  },
  {
    "name": "cradily",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/346.png",
    "types": [
      "rock",
      "grass"
    ]
  },
  {
    "name": "anorith",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/347.png",
    "types": [
      "rock",
      "bug"
    ]
  },
  {
    "name": "armaldo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/348.png",
    "types": [
      "rock",
      "bug"
    ]
  },
  {
    "name": "feebas",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/349.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "milotic",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/350.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "castform",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/351.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "kecleon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/352.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "shuppet",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/353.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "banette",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/354.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "duskull",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/355.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "dusclops",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/356.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "tropius",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/357.png",
    "types": [
      "grass",
      "flying"
    ]
  },
  {
    "name": "chimecho",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/358.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "absol",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/359.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "wynaut",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/360.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "snorunt",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/361.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "glalie",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/362.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "spheal",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/363.png",
    "types": [
      "ice",
      "water"
    ]
  },
  {
    "name": "sealeo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/364.png",
    "types": [
      "ice",
      "water"
    ]
  },
  {
    "name": "walrein",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/365.png",
    "types": [
      "ice",
      "water"
    ]
  },
  {
    "name": "clamperl",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/366.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "huntail",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/367.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "gorebyss",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/368.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "relicanth",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/369.png",
    "types": [
      "water",
      "rock"
    ]
  },
  {
    "name": "luvdisc",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/370.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "bagon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/371.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "shelgon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/372.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "salamence",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/373.png",
    "types": [
      "dragon",
      "flying"
    ]
  },
  {
    "name": "beldum",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/374.png",
    "types": [
      "steel",
      "psychic"
    ]
  },
  {
    "name": "metang",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/375.png",
    "types": [
      "steel",
      "psychic"
    ]
  },
  {
    "name": "metagross",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/376.png",
    "types": [
      "steel",
      "psychic"
    ]
  },
  {
    "name": "regirock",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/377.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "regice",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/378.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "registeel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/379.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "latias",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/380.png",
    "types": [
      "dragon",
      "psychic"
    ]
  },
  {
    "name": "latios",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/381.png",
    "types": [
      "dragon",
      "psychic"
    ]
  },
  {
    "name": "kyogre",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/382.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "groudon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/383.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "rayquaza",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/384.png",
    "types": [
      "dragon",
      "flying"
    ]
  },
  {
    "name": "jirachi",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/385.png",
    "types": [
      "steel",
      "psychic"
    ]
  },
  {
    "name": "deoxys-normal",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/386.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "turtwig",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/387.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "grotle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/388.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "torterra",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/389.png",
    "types": [
      "grass",
      "ground"
    ]
  },
  {
    "name": "chimchar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/390.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "monferno",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/391.png",
    "types": [
      "fire",
      "fighting"
    ]
  },
  {
    "name": "infernape",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/392.png",
    "types": [
      "fire",
      "fighting"
    ]
  },
  {
    "name": "piplup",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/393.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "prinplup",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/394.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "empoleon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/395.png",
    "types": [
      "water",
      "steel"
    ]
  },
  {
    "name": "starly",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/396.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "staravia",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/397.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "staraptor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/398.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "bidoof",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/399.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "bibarel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/400.png",
    "types": [
      "normal",
      "water"
    ]
  },
  {
    "name": "kricketot",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/401.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "kricketune",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/402.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "shinx",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/403.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "luxio",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/404.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "luxray",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/405.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "budew",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/406.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "roserade",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/407.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "cranidos",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/408.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "rampardos",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/409.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "shieldon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/410.png",
    "types": [
      "rock",
      "steel"
    ]
  },
  {
    "name": "bastiodon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/411.png",
    "types": [
      "rock",
      "steel"
    ]
  },
  {
    "name": "burmy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/412.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "wormadam-plant",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/413.png",
    "types": [
      "bug",
      "grass"
    ]
  },
  {
    "name": "mothim",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/414.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "combee",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/415.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "vespiquen",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/416.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "pachirisu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/417.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "buizel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/418.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "floatzel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/419.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "cherubi",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/420.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "cherrim",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/421.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "shellos",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/422.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "gastrodon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/423.png",
    "types": [
      "water",
      "ground"
    ]
  },
  {
    "name": "ambipom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/424.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "drifloon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/425.png",
    "types": [
      "ghost",
      "flying"
    ]
  },
  {
    "name": "drifblim",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/426.png",
    "types": [
      "ghost",
      "flying"
    ]
  },
  {
    "name": "buneary",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/427.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "lopunny",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/428.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "mismagius",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/429.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "honchkrow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/430.png",
    "types": [
      "dark",
      "flying"
    ]
  },
  {
    "name": "glameow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/431.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "purugly",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/432.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "chingling",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/433.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "stunky",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/434.png",
    "types": [
      "poison",
      "dark"
    ]
  },
  {
    "name": "skuntank",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/435.png",
    "types": [
      "poison",
      "dark"
    ]
  },
  {
    "name": "bronzor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/436.png",
    "types": [
      "steel",
      "psychic"
    ]
  },
  {
    "name": "bronzong",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/437.png",
    "types": [
      "steel",
      "psychic"
    ]
  },
  {
    "name": "bonsly",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/438.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "mime-jr",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/439.png",
    "types": [
      "psychic",
      "fairy"
    ]
  },
  {
    "name": "happiny",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/440.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "chatot",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/441.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "spiritomb",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/442.png",
    "types": [
      "ghost",
      "dark"
    ]
  },
  {
    "name": "gible",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/443.png",
    "types": [
      "dragon",
      "ground"
    ]
  },
  {
    "name": "gabite",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/444.png",
    "types": [
      "dragon",
      "ground"
    ]
  },
  {
    "name": "garchomp",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/445.png",
    "types": [
      "dragon",
      "ground"
    ]
  },
  {
    "name": "munchlax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/446.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "riolu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/447.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "lucario",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/448.png",
    "types": [
      "fighting",
      "steel"
    ]
  },
  {
    "name": "hippopotas",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/449.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "hippowdon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/450.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "skorupi",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/451.png",
    "types": [
      "poison",
      "bug"
    ]
  },
  {
    "name": "drapion",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/452.png",
    "types": [
      "poison",
      "dark"
    ]
  },
  {
    "name": "croagunk",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/453.png",
    "types": [
      "poison",
      "fighting"
    ]
  },
  {
    "name": "toxicroak",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/454.png",
    "types": [
      "poison",
      "fighting"
    ]
  },
  {
    "name": "carnivine",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/455.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "finneon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/456.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "lumineon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/457.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "mantyke",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/458.png",
    "types": [
      "water",
      "flying"
    ]
  },
  {
    "name": "snover",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/459.png",
    "types": [
      "grass",
      "ice"
    ]
  },
  {
    "name": "abomasnow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/460.png",
    "types": [
      "grass",
      "ice"
    ]
  },
  {
    "name": "weavile",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/461.png",
    "types": [
      "dark",
      "ice"
    ]
  },
  {
    "name": "magnezone",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/462.png",
    "types": [
      "electric",
      "steel"
    ]
  },
  {
    "name": "lickilicky",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/463.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "rhyperior",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/464.png",
    "types": [
      "ground",
      "rock"
    ]
  },
  {
    "name": "tangrowth",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/465.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "electivire",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/466.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "magmortar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/467.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "togekiss",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/468.png",
    "types": [
      "fairy",
      "flying"
    ]
  },
  {
    "name": "yanmega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/469.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "leafeon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/470.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "glaceon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/471.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "gliscor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/472.png",
    "types": [
      "ground",
      "flying"
    ]
  },
  {
    "name": "mamoswine",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/473.png",
    "types": [
      "ice",
      "ground"
    ]
  },
  {
    "name": "porygon-z",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/474.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "gallade",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/475.png",
    "types": [
      "psychic",
      "fighting"
    ]
  },
  {
    "name": "probopass",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/476.png",
    "types": [
      "rock",
      "steel"
    ]
  },
  {
    "name": "dusknoir",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/477.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "froslass",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/478.png",
    "types": [
      "ice",
      "ghost"
    ]
  },
  {
    "name": "rotom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/479.png",
    "types": [
      "electric",
      "ghost"
    ]
  },
  {
    "name": "uxie",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/480.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "mesprit",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/481.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "azelf",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/482.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "dialga",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/483.png",
    "types": [
      "steel",
      "dragon"
    ]
  },
  {
    "name": "palkia",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/484.png",
    "types": [
      "water",
      "dragon"
    ]
  },
  {
    "name": "heatran",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/485.png",
    "types": [
      "fire",
      "steel"
    ]
  },
  {
    "name": "regigigas",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/486.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "giratina-altered",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/487.png",
    "types": [
      "ghost",
      "dragon"
    ]
  },
  {
    "name": "cresselia",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/488.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "phione",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/489.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "manaphy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/490.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "darkrai",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/491.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "shaymin-land",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/492.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "arceus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/493.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "victini",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/494.png",
    "types": [
      "psychic",
      "fire"
    ]
  },
  {
    "name": "snivy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/495.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "servine",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/496.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "serperior",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/497.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "tepig",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/498.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "pignite",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/499.png",
    "types": [
      "fire",
      "fighting"
    ]
  },
  {
    "name": "emboar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/500.png",
    "types": [
      "fire",
      "fighting"
    ]
  },
  {
    "name": "oshawott",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/501.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "dewott",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/502.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "samurott",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/503.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "patrat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/504.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "watchog",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/505.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "lillipup",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/506.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "herdier",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/507.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "stoutland",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/508.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "purrloin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/509.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "liepard",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/510.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "pansage",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/511.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "simisage",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/512.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "pansear",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/513.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "simisear",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/514.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "panpour",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/515.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "simipour",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/516.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "munna",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/517.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "musharna",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/518.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "pidove",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/519.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "tranquill",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/520.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "unfezant",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/521.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "blitzle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/522.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "zebstrika",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/523.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "roggenrola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/524.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "boldore",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/525.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "gigalith",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/526.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "woobat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/527.png",
    "types": [
      "psychic",
      "flying"
    ]
  },
  {
    "name": "swoobat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/528.png",
    "types": [
      "psychic",
      "flying"
    ]
  },
  {
    "name": "drilbur",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/529.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "excadrill",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/530.png",
    "types": [
      "ground",
      "steel"
    ]
  },
  {
    "name": "audino",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/531.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "timburr",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/532.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "gurdurr",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/533.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "conkeldurr",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/534.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "tympole",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/535.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "palpitoad",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/536.png",
    "types": [
      "water",
      "ground"
    ]
  },
  {
    "name": "seismitoad",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/537.png",
    "types": [
      "water",
      "ground"
    ]
  },
  {
    "name": "throh",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/538.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "sawk",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/539.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "sewaddle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/540.png",
    "types": [
      "bug",
      "grass"
    ]
  },
  {
    "name": "swadloon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/541.png",
    "types": [
      "bug",
      "grass"
    ]
  },
  {
    "name": "leavanny",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/542.png",
    "types": [
      "bug",
      "grass"
    ]
  },
  {
    "name": "venipede",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/543.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "whirlipede",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/544.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "scolipede",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/545.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "cottonee",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/546.png",
    "types": [
      "grass",
      "fairy"
    ]
  },
  {
    "name": "whimsicott",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/547.png",
    "types": [
      "grass",
      "fairy"
    ]
  },
  {
    "name": "petilil",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/548.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "lilligant",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/549.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "basculin-red-striped",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/550.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "sandile",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/551.png",
    "types": [
      "ground",
      "dark"
    ]
  },
  {
    "name": "krokorok",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/552.png",
    "types": [
      "ground",
      "dark"
    ]
  },
  {
    "name": "krookodile",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/553.png",
    "types": [
      "ground",
      "dark"
    ]
  },
  {
    "name": "darumaka",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/554.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "darmanitan-standard",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/555.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "maractus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/556.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "dwebble",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/557.png",
    "types": [
      "bug",
      "rock"
    ]
  },
  {
    "name": "crustle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/558.png",
    "types": [
      "bug",
      "rock"
    ]
  },
  {
    "name": "scraggy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/559.png",
    "types": [
      "dark",
      "fighting"
    ]
  },
  {
    "name": "scrafty",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/560.png",
    "types": [
      "dark",
      "fighting"
    ]
  },
  {
    "name": "sigilyph",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/561.png",
    "types": [
      "psychic",
      "flying"
    ]
  },
  {
    "name": "yamask",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/562.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "cofagrigus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/563.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "tirtouga",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/564.png",
    "types": [
      "water",
      "rock"
    ]
  },
  {
    "name": "carracosta",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/565.png",
    "types": [
      "water",
      "rock"
    ]
  },
  {
    "name": "archen",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/566.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "archeops",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/567.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "trubbish",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/568.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "garbodor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/569.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "zorua",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/570.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "zoroark",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/571.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "minccino",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/572.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "cinccino",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/573.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "gothita",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/574.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "gothorita",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/575.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "gothitelle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/576.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "solosis",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/577.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "duosion",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/578.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "reuniclus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/579.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "ducklett",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/580.png",
    "types": [
      "water",
      "flying"
    ]
  },
  {
    "name": "swanna",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/581.png",
    "types": [
      "water",
      "flying"
    ]
  },
  {
    "name": "vanillite",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/582.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "vanillish",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/583.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "vanilluxe",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/584.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "deerling",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/585.png",
    "types": [
      "normal",
      "grass"
    ]
  },
  {
    "name": "sawsbuck",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/586.png",
    "types": [
      "normal",
      "grass"
    ]
  },
  {
    "name": "emolga",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/587.png",
    "types": [
      "electric",
      "flying"
    ]
  },
  {
    "name": "karrablast",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/588.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "escavalier",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/589.png",
    "types": [
      "bug",
      "steel"
    ]
  },
  {
    "name": "foongus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/590.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "amoonguss",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/591.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "frillish",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/592.png",
    "types": [
      "water",
      "ghost"
    ]
  },
  {
    "name": "jellicent",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/593.png",
    "types": [
      "water",
      "ghost"
    ]
  },
  {
    "name": "alomomola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/594.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "joltik",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/595.png",
    "types": [
      "bug",
      "electric"
    ]
  },
  {
    "name": "galvantula",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/596.png",
    "types": [
      "bug",
      "electric"
    ]
  },
  {
    "name": "ferroseed",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/597.png",
    "types": [
      "grass",
      "steel"
    ]
  },
  {
    "name": "ferrothorn",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/598.png",
    "types": [
      "grass",
      "steel"
    ]
  },
  {
    "name": "klink",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/599.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "klang",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/600.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "klinklang",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/601.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "tynamo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/602.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "eelektrik",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/603.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "eelektross",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/604.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "elgyem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/605.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "beheeyem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/606.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "litwick",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/607.png",
    "types": [
      "ghost",
      "fire"
    ]
  },
  {
    "name": "lampent",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/608.png",
    "types": [
      "ghost",
      "fire"
    ]
  },
  {
    "name": "chandelure",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/609.png",
    "types": [
      "ghost",
      "fire"
    ]
  },
  {
    "name": "axew",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/610.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "fraxure",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/611.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "haxorus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/612.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "cubchoo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/613.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "beartic",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/614.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "cryogonal",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/615.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "shelmet",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/616.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "accelgor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/617.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "stunfisk",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/618.png",
    "types": [
      "ground",
      "electric"
    ]
  },
  {
    "name": "mienfoo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/619.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "mienshao",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/620.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "druddigon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/621.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "golett",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/622.png",
    "types": [
      "ground",
      "ghost"
    ]
  },
  {
    "name": "golurk",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/623.png",
    "types": [
      "ground",
      "ghost"
    ]
  },
  {
    "name": "pawniard",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/624.png",
    "types": [
      "dark",
      "steel"
    ]
  },
  {
    "name": "bisharp",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/625.png",
    "types": [
      "dark",
      "steel"
    ]
  },
  {
    "name": "bouffalant",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/626.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "rufflet",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/627.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "braviary",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/628.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "vullaby",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/629.png",
    "types": [
      "dark",
      "flying"
    ]
  },
  {
    "name": "mandibuzz",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/630.png",
    "types": [
      "dark",
      "flying"
    ]
  },
  {
    "name": "heatmor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/631.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "durant",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/632.png",
    "types": [
      "bug",
      "steel"
    ]
  },
  {
    "name": "deino",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/633.png",
    "types": [
      "dark",
      "dragon"
    ]
  },
  {
    "name": "zweilous",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/634.png",
    "types": [
      "dark",
      "dragon"
    ]
  },
  {
    "name": "hydreigon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/635.png",
    "types": [
      "dark",
      "dragon"
    ]
  },
  {
    "name": "larvesta",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/636.png",
    "types": [
      "bug",
      "fire"
    ]
  },
  {
    "name": "volcarona",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/637.png",
    "types": [
      "bug",
      "fire"
    ]
  },
  {
    "name": "cobalion",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/638.png",
    "types": [
      "steel",
      "fighting"
    ]
  },
  {
    "name": "terrakion",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/639.png",
    "types": [
      "rock",
      "fighting"
    ]
  },
  {
    "name": "virizion",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/640.png",
    "types": [
      "grass",
      "fighting"
    ]
  },
  {
    "name": "tornadus-incarnate",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/641.png",
    "types": [
      "flying"
    ]
  },
  {
    "name": "thundurus-incarnate",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/642.png",
    "types": [
      "electric",
      "flying"
    ]
  },
  {
    "name": "reshiram",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/643.png",
    "types": [
      "dragon",
      "fire"
    ]
  },
  {
    "name": "zekrom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/644.png",
    "types": [
      "dragon",
      "electric"
    ]
  },
  {
    "name": "landorus-incarnate",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/645.png",
    "types": [
      "ground",
      "flying"
    ]
  },
  {
    "name": "kyurem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/646.png",
    "types": [
      "dragon",
      "ice"
    ]
  },
  {
    "name": "keldeo-ordinary",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/647.png",
    "types": [
      "water",
      "fighting"
    ]
  },
  {
    "name": "meloetta-aria",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/648.png",
    "types": [
      "normal",
      "psychic"
    ]
  },
  {
    "name": "genesect",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/649.png",
    "types": [
      "bug",
      "steel"
    ]
  },
  {
    "name": "chespin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/650.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "quilladin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/651.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "chesnaught",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/652.png",
    "types": [
      "grass",
      "fighting"
    ]
  },
  {
    "name": "fennekin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/653.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "braixen",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/654.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "delphox",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/655.png",
    "types": [
      "fire",
      "psychic"
    ]
  },
  {
    "name": "froakie",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/656.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "frogadier",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/657.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "greninja",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/658.png",
    "types": [
      "water",
      "dark"
    ]
  },
  {
    "name": "bunnelby",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/659.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "diggersby",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/660.png",
    "types": [
      "normal",
      "ground"
    ]
  },
  {
    "name": "fletchling",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/661.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "fletchinder",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/662.png",
    "types": [
      "fire",
      "flying"
    ]
  },
  {
    "name": "talonflame",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/663.png",
    "types": [
      "fire",
      "flying"
    ]
  },
  {
    "name": "scatterbug",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/664.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "spewpa",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/665.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "vivillon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/666.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "litleo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/667.png",
    "types": [
      "fire",
      "normal"
    ]
  },
  {
    "name": "pyroar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/668.png",
    "types": [
      "fire",
      "normal"
    ]
  },
  {
    "name": "flabebe",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/669.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "floette",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/670.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "florges",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/671.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "skiddo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/672.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "gogoat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/673.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "pancham",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/674.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "pangoro",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/675.png",
    "types": [
      "fighting",
      "dark"
    ]
  },
  {
    "name": "furfrou",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/676.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "espurr",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/677.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "meowstic-male",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/678.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "honedge",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/679.png",
    "types": [
      "steel",
      "ghost"
    ]
  },
  {
    "name": "doublade",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/680.png",
    "types": [
      "steel",
      "ghost"
    ]
  },
  {
    "name": "aegislash-shield",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/681.png",
    "types": [
      "steel",
      "ghost"
    ]
  },
  {
    "name": "spritzee",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/682.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "aromatisse",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/683.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "swirlix",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/684.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "slurpuff",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/685.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "inkay",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/686.png",
    "types": [
      "dark",
      "psychic"
    ]
  },
  {
    "name": "malamar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/687.png",
    "types": [
      "dark",
      "psychic"
    ]
  },
  {
    "name": "binacle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/688.png",
    "types": [
      "rock",
      "water"
    ]
  },
  {
    "name": "barbaracle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/689.png",
    "types": [
      "rock",
      "water"
    ]
  },
  {
    "name": "skrelp",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/690.png",
    "types": [
      "poison",
      "water"
    ]
  },
  {
    "name": "dragalge",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/691.png",
    "types": [
      "poison",
      "dragon"
    ]
  },
  {
    "name": "clauncher",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/692.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "clawitzer",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/693.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "helioptile",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/694.png",
    "types": [
      "electric",
      "normal"
    ]
  },
  {
    "name": "heliolisk",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/695.png",
    "types": [
      "electric",
      "normal"
    ]
  },
  {
    "name": "tyrunt",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/696.png",
    "types": [
      "rock",
      "dragon"
    ]
  },
  {
    "name": "tyrantrum",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/697.png",
    "types": [
      "rock",
      "dragon"
    ]
  },
  {
    "name": "amaura",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/698.png",
    "types": [
      "rock",
      "ice"
    ]
  },
  {
    "name": "aurorus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/699.png",
    "types": [
      "rock",
      "ice"
    ]
  },
  {
    "name": "sylveon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/700.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "hawlucha",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/701.png",
    "types": [
      "fighting",
      "flying"
    ]
  },
  {
    "name": "dedenne",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/702.png",
    "types": [
      "electric",
      "fairy"
    ]
  },
  {
    "name": "carbink",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/703.png",
    "types": [
      "rock",
      "fairy"
    ]
  },
  {
    "name": "goomy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/704.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "sliggoo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/705.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "goodra",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/706.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "klefki",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/707.png",
    "types": [
      "steel",
      "fairy"
    ]
  },
  {
    "name": "phantump",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/708.png",
    "types": [
      "ghost",
      "grass"
    ]
  },
  {
    "name": "trevenant",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/709.png",
    "types": [
      "ghost",
      "grass"
    ]
  },
  {
    "name": "pumpkaboo-average",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/710.png",
    "types": [
      "ghost",
      "grass"
    ]
  },
  {
    "name": "gourgeist-average",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/711.png",
    "types": [
      "ghost",
      "grass"
    ]
  },
  {
    "name": "bergmite",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/712.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "avalugg",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/713.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "noibat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/714.png",
    "types": [
      "flying",
      "dragon"
    ]
  },
  {
    "name": "noivern",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/715.png",
    "types": [
      "flying",
      "dragon"
    ]
  },
  {
    "name": "xerneas",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/716.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "yveltal",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/717.png",
    "types": [
      "dark",
      "flying"
    ]
  },
  {
    "name": "zygarde-50",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/718.png",
    "types": [
      "dragon",
      "ground"
    ]
  },
  {
    "name": "diancie",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/719.png",
    "types": [
      "rock",
      "fairy"
    ]
  },
  {
    "name": "hoopa",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/720.png",
    "types": [
      "psychic",
      "ghost"
    ]
  },
  {
    "name": "volcanion",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/721.png",
    "types": [
      "fire",
      "water"
    ]
  },
  {
    "name": "rowlet",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/722.png",
    "types": [
      "grass",
      "flying"
    ]
  },
  {
    "name": "dartrix",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/723.png",
    "types": [
      "grass",
      "flying"
    ]
  },
  {
    "name": "decidueye",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/724.png",
    "types": [
      "grass",
      "ghost"
    ]
  },
  {
    "name": "litten",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/725.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "torracat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/726.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "incineroar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/727.png",
    "types": [
      "fire",
      "dark"
    ]
  },
  {
    "name": "popplio",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/728.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "brionne",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/729.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "primarina",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/730.png",
    "types": [
      "water",
      "fairy"
    ]
  },
  {
    "name": "pikipek",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/731.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "trumbeak",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/732.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "toucannon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/733.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "yungoos",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/734.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "gumshoos",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/735.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "grubbin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/736.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "charjabug",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/737.png",
    "types": [
      "bug",
      "electric"
    ]
  },
  {
    "name": "vikavolt",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/738.png",
    "types": [
      "bug",
      "electric"
    ]
  },
  {
    "name": "crabrawler",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/739.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "crabominable",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/740.png",
    "types": [
      "fighting",
      "ice"
    ]
  },
  {
    "name": "oricorio-baile",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/741.png",
    "types": [
      "fire",
      "flying"
    ]
  },
  {
    "name": "cutiefly",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/742.png",
    "types": [
      "bug",
      "fairy"
    ]
  },
  {
    "name": "ribombee",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/743.png",
    "types": [
      "bug",
      "fairy"
    ]
  },
  {
    "name": "rockruff",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/744.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "lycanroc-midday",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/745.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "wishiwashi-solo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/746.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "mareanie",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/747.png",
    "types": [
      "poison",
      "water"
    ]
  },
  {
    "name": "toxapex",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/748.png",
    "types": [
      "poison",
      "water"
    ]
  },
  {
    "name": "mudbray",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/749.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "mudsdale",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/750.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "dewpider",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/751.png",
    "types": [
      "water",
      "bug"
    ]
  },
  {
    "name": "araquanid",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/752.png",
    "types": [
      "water",
      "bug"
    ]
  },
  {
    "name": "fomantis",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/753.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "lurantis",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/754.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "morelull",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/755.png",
    "types": [
      "grass",
      "fairy"
    ]
  },
  {
    "name": "shiinotic",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/756.png",
    "types": [
      "grass",
      "fairy"
    ]
  },
  {
    "name": "salandit",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/757.png",
    "types": [
      "poison",
      "fire"
    ]
  },
  {
    "name": "salazzle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/758.png",
    "types": [
      "poison",
      "fire"
    ]
  },
  {
    "name": "stufful",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/759.png",
    "types": [
      "normal",
      "fighting"
    ]
  },
  {
    "name": "bewear",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/760.png",
    "types": [
      "normal",
      "fighting"
    ]
  },
  {
    "name": "bounsweet",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/761.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "steenee",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/762.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "tsareena",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/763.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "comfey",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/764.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "oranguru",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/765.png",
    "types": [
      "normal",
      "psychic"
    ]
  },
  {
    "name": "passimian",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/766.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "wimpod",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/767.png",
    "types": [
      "bug",
      "water"
    ]
  },
  {
    "name": "golisopod",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/768.png",
    "types": [
      "bug",
      "water"
    ]
  },
  {
    "name": "sandygast",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/769.png",
    "types": [
      "ghost",
      "ground"
    ]
  },
  {
    "name": "palossand",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/770.png",
    "types": [
      "ghost",
      "ground"
    ]
  },
  {
    "name": "pyukumuku",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/771.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "type-null",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/772.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "silvally",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/773.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "minior-red-meteor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/774.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "komala",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/775.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "turtonator",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/776.png",
    "types": [
      "fire",
      "dragon"
    ]
  },
  {
    "name": "togedemaru",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/777.png",
    "types": [
      "electric",
      "steel"
    ]
  },
  {
    "name": "mimikyu-disguised",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/778.png",
    "types": [
      "ghost",
      "fairy"
    ]
  },
  {
    "name": "bruxish",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/779.png",
    "types": [
      "water",
      "psychic"
    ]
  },
  {
    "name": "drampa",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/780.png",
    "types": [
      "normal",
      "dragon"
    ]
  },
  {
    "name": "dhelmise",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/781.png",
    "types": [
      "ghost",
      "grass"
    ]
  },
  {
    "name": "jangmo-o",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/782.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "hakamo-o",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/783.png",
    "types": [
      "dragon",
      "fighting"
    ]
  },
  {
    "name": "kommo-o",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/784.png",
    "types": [
      "dragon",
      "fighting"
    ]
  },
  {
    "name": "tapu-koko",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/785.png",
    "types": [
      "electric",
      "fairy"
    ]
  },
  {
    "name": "tapu-lele",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/786.png",
    "types": [
      "psychic",
      "fairy"
    ]
  },
  {
    "name": "tapu-bulu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/787.png",
    "types": [
      "grass",
      "fairy"
    ]
  },
  {
    "name": "tapu-fini",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/788.png",
    "types": [
      "water",
      "fairy"
    ]
  },
  {
    "name": "cosmog",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/789.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "cosmoem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/790.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "solgaleo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/791.png",
    "types": [
      "psychic",
      "steel"
    ]
  },
  {
    "name": "lunala",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/792.png",
    "types": [
      "psychic",
      "ghost"
    ]
  },
  {
    "name": "nihilego",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/793.png",
    "types": [
      "rock",
      "poison"
    ]
  },
  {
    "name": "buzzwole",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/794.png",
    "types": [
      "bug",
      "fighting"
    ]
  },
  {
    "name": "pheromosa",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/795.png",
    "types": [
      "bug",
      "fighting"
    ]
  },
  {
    "name": "xurkitree",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/796.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "celesteela",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/797.png",
    "types": [
      "steel",
      "flying"
    ]
  },
  {
    "name": "kartana",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/798.png",
    "types": [
      "grass",
      "steel"
    ]
  },
  {
    "name": "guzzlord",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/799.png",
    "types": [
      "dark",
      "dragon"
    ]
  },
  {
    "name": "necrozma",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/800.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "magearna",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/801.png",
    "types": [
      "steel",
      "fairy"
    ]
  },
  {
    "name": "marshadow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/802.png",
    "types": [
      "fighting",
      "ghost"
    ]
  },
  {
    "name": "poipole",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/803.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "naganadel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/804.png",
    "types": [
      "poison",
      "dragon"
    ]
  },
  {
    "name": "stakataka",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/805.png",
    "types": [
      "rock",
      "steel"
    ]
  },
  {
    "name": "blacephalon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/806.png",
    "types": [
      "fire",
      "ghost"
    ]
  },
  {
    "name": "zeraora",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/807.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "meltan",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/808.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "melmetal",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/809.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "grookey",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/810.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "thwackey",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/811.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "rillaboom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/812.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "scorbunny",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/813.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "raboot",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/814.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "cinderace",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/815.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "sobble",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/816.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "drizzile",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/817.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "inteleon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/818.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "skwovet",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/819.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "greedent",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/820.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "rookidee",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/821.png",
    "types": [
      "flying"
    ]
  },
  {
    "name": "corvisquire",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/822.png",
    "types": [
      "flying"
    ]
  },
  {
    "name": "corviknight",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/823.png",
    "types": [
      "flying",
      "steel"
    ]
  },
  {
    "name": "blipbug",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/824.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "dottler",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/825.png",
    "types": [
      "bug",
      "psychic"
    ]
  },
  {
    "name": "orbeetle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/826.png",
    "types": [
      "bug",
      "psychic"
    ]
  },
  {
    "name": "nickit",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/827.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "thievul",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/828.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "gossifleur",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/829.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "eldegoss",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/830.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "wooloo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/831.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "dubwool",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/832.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "chewtle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/833.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "drednaw",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/834.png",
    "types": [
      "water",
      "rock"
    ]
  },
  {
    "name": "yamper",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/835.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "boltund",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/836.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "rolycoly",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/837.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "carkol",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/838.png",
    "types": [
      "rock",
      "fire"
    ]
  },
  {
    "name": "coalossal",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/839.png",
    "types": [
      "rock",
      "fire"
    ]
  },
  {
    "name": "applin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/840.png",
    "types": [
      "grass",
      "dragon"
    ]
  },
  {
    "name": "flapple",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/841.png",
    "types": [
      "grass",
      "dragon"
    ]
  },
  {
    "name": "appletun",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/842.png",
    "types": [
      "grass",
      "dragon"
    ]
  },
  {
    "name": "silicobra",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/843.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "sandaconda",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/844.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "cramorant",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/845.png",
    "types": [
      "flying",
      "water"
    ]
  },
  {
    "name": "arrokuda",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/846.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "barraskewda",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/847.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "toxel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/848.png",
    "types": [
      "electric",
      "poison"
    ]
  },
  {
    "name": "toxtricity-amped",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/849.png",
    "types": [
      "electric",
      "poison"
    ]
  },
  {
    "name": "sizzlipede",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/850.png",
    "types": [
      "fire",
      "bug"
    ]
  },
  {
    "name": "centiskorch",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/851.png",
    "types": [
      "fire",
      "bug"
    ]
  },
  {
    "name": "clobbopus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/852.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "grapploct",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/853.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "sinistea",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/854.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "polteageist",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/855.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "hatenna",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/856.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "hattrem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/857.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "hatterene",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/858.png",
    "types": [
      "psychic",
      "fairy"
    ]
  },
  {
    "name": "impidimp",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/859.png",
    "types": [
      "dark",
      "fairy"
    ]
  },
  {
    "name": "morgrem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/860.png",
    "types": [
      "dark",
      "fairy"
    ]
  },
  {
    "name": "grimmsnarl",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/861.png",
    "types": [
      "dark",
      "fairy"
    ]
  },
  {
    "name": "obstagoon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/862.png",
    "types": [
      "dark",
      "normal"
    ]
  },
  {
    "name": "perrserker",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/863.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "cursola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/864.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "sirfetchd",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/865.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "mr-rime",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/866.png",
    "types": [
      "ice",
      "psychic"
    ]
  },
  {
    "name": "runerigus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/867.png",
    "types": [
      "ground",
      "ghost"
    ]
  },
  {
    "name": "milcery",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/868.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "alcremie",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/869.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "falinks",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/870.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "pincurchin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/871.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "snom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/872.png",
    "types": [
      "ice",
      "bug"
    ]
  },
  {
    "name": "frosmoth",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/873.png",
    "types": [
      "ice",
      "bug"
    ]
  },
  {
    "name": "stonjourner",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/874.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "eiscue-ice",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/875.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "indeedee-male",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/876.png",
    "types": [
      "psychic",
      "normal"
    ]
  },
  {
    "name": "morpeko-full-belly",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/877.png",
    "types": [
      "electric",
      "dark"
    ]
  },
  {
    "name": "cufant",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/878.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "copperajah",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/879.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "dracozolt",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/880.png",
    "types": [
      "electric",
      "dragon"
    ]
  },
  {
    "name": "arctozolt",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/881.png",
    "types": [
      "electric",
      "ice"
    ]
  },
  {
    "name": "dracovish",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/882.png",
    "types": [
      "water",
      "dragon"
    ]
  },
  {
    "name": "arctovish",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/883.png",
    "types": [
      "water",
      "ice"
    ]
  },
  {
    "name": "duraludon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/884.png",
    "types": [
      "steel",
      "dragon"
    ]
  },
  {
    "name": "dreepy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/885.png",
    "types": [
      "dragon",
      "ghost"
    ]
  },
  {
    "name": "drakloak",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/886.png",
    "types": [
      "dragon",
      "ghost"
    ]
  },
  {
    "name": "dragapult",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/887.png",
    "types": [
      "dragon",
      "ghost"
    ]
  },
  {
    "name": "zacian",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/888.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "zamazenta",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/889.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "eternatus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/890.png",
    "types": [
      "poison",
      "dragon"
    ]
  },
  {
    "name": "kubfu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/891.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "urshifu-single-strike",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/892.png",
    "types": [
      "fighting",
      "dark"
    ]
  },
  {
    "name": "zarude",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/893.png",
    "types": [
      "dark",
      "grass"
    ]
  },
  {
    "name": "regieleki",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/894.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "regidrago",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/895.png",
    "types": [
      "dragon"
    ]
  },
  {
    "name": "glastrier",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/896.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "spectrier",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/897.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "calyrex",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/898.png",
    "types": [
      "psychic",
      "grass"
    ]
  },
  {
    "name": "wyrdeer",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/899.png",
    "types": [
      "normal",
      "psychic"
    ]
  },
  {
    "name": "kleavor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/900.png",
    "types": [
      "bug",
      "rock"
    ]
  },
  {
    "name": "ursaluna",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/901.png",
    "types": [
      "ground",
      "normal"
    ]
  },
  {
    "name": "basculegion-male",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/902.png",
    "types": [
      "water",
      "ghost"
    ]
  },
  {
    "name": "sneasler",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/903.png",
    "types": [
      "fighting",
      "poison"
    ]
  },
  {
    "name": "overqwil",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/904.png",
    "types": [
      "dark",
      "poison"
    ]
  },
  {
    "name": "enamorus-incarnate",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/905.png",
    "types": [
      "fairy",
      "flying"
    ]
  },
  {
    "name": "sprigatito",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/906.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "floragato",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/907.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "meowscarada",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/908.png",
    "types": [
      "grass",
      "dark"
    ]
  },
  {
    "name": "fuecoco",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/909.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "crocalor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/910.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "skeledirge",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/911.png",
    "types": [
      "fire",
      "ghost"
    ]
  },
  {
    "name": "quaxly",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/912.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "quaxwell",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/913.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "quaquaval",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/914.png",
    "types": [
      "water",
      "fighting"
    ]
  },
  {
    "name": "lechonk",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/915.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "oinkologne-male",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/916.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "tarountula",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/917.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "spidops",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/918.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "nymble",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/919.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "lokix",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/920.png",
    "types": [
      "bug",
      "dark"
    ]
  },
  {
    "name": "pawmi",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/921.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "pawmo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/922.png",
    "types": [
      "electric",
      "fighting"
    ]
  },
  {
    "name": "pawmot",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/923.png",
    "types": [
      "electric",
      "fighting"
    ]
  },
  {
    "name": "tandemaus",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/924.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "maushold-family-of-four",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/925.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "fidough",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/926.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "dachsbun",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/927.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "smoliv",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/928.png",
    "types": [
      "grass",
      "normal"
    ]
  },
  {
    "name": "dolliv",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/929.png",
    "types": [
      "grass",
      "normal"
    ]
  },
  {
    "name": "arboliva",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/930.png",
    "types": [
      "grass",
      "normal"
    ]
  },
  {
    "name": "squawkabilly-green-plumage",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/931.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "nacli",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/932.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "naclstack",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/933.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "garganacl",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/934.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "charcadet",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/935.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "armarouge",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/936.png",
    "types": [
      "fire",
      "psychic"
    ]
  },
  {
    "name": "ceruledge",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/937.png",
    "types": [
      "fire",
      "ghost"
    ]
  },
  {
    "name": "tadbulb",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/938.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "bellibolt",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/939.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "wattrel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/940.png",
    "types": [
      "electric",
      "flying"
    ]
  },
  {
    "name": "kilowattrel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/941.png",
    "types": [
      "electric",
      "flying"
    ]
  },
  {
    "name": "maschiff",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/942.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "mabosstiff",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/943.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "shroodle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/944.png",
    "types": [
      "poison",
      "normal"
    ]
  },
  {
    "name": "grafaiai",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/945.png",
    "types": [
      "poison",
      "normal"
    ]
  },
  {
    "name": "bramblin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/946.png",
    "types": [
      "grass",
      "ghost"
    ]
  },
  {
    "name": "brambleghast",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/947.png",
    "types": [
      "grass",
      "ghost"
    ]
  },
  {
    "name": "toedscool",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/948.png",
    "types": [
      "ground",
      "grass"
    ]
  },
  {
    "name": "toedscruel",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/949.png",
    "types": [
      "ground",
      "grass"
    ]
  },
  {
    "name": "klawf",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/950.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "capsakid",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/951.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "scovillain",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/952.png",
    "types": [
      "grass",
      "fire"
    ]
  },
  {
    "name": "rellor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/953.png",
    "types": [
      "bug"
    ]
  },
  {
    "name": "rabsca",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/954.png",
    "types": [
      "bug",
      "psychic"
    ]
  },
  {
    "name": "flittle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/955.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "espathra",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/956.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "tinkatink",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/957.png",
    "types": [
      "fairy",
      "steel"
    ]
  },
  {
    "name": "tinkatuff",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/958.png",
    "types": [
      "fairy",
      "steel"
    ]
  },
  {
    "name": "tinkaton",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/959.png",
    "types": [
      "fairy",
      "steel"
    ]
  },
  {
    "name": "wiglett",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/960.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "wugtrio",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/961.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "bombirdier",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/962.png",
    "types": [
      "flying",
      "dark"
    ]
  },
  {
    "name": "finizen",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/963.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "palafin-zero",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/964.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "varoom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/965.png",
    "types": [
      "steel",
      "poison"
    ]
  },
  {
    "name": "revavroom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/966.png",
    "types": [
      "steel",
      "poison"
    ]
  },
  {
    "name": "cyclizar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/967.png",
    "types": [
      "dragon",
      "normal"
    ]
  },
  {
    "name": "orthworm",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/968.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "glimmet",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/969.png",
    "types": [
      "rock",
      "poison"
    ]
  },
  {
    "name": "glimmora",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/970.png",
    "types": [
      "rock",
      "poison"
    ]
  },
  {
    "name": "greavard",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/971.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "houndstone",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/972.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "flamigo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/973.png",
    "types": [
      "flying",
      "fighting"
    ]
  },
  {
    "name": "cetoddle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/974.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "cetitan",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/975.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "veluza",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/976.png",
    "types": [
      "water",
      "psychic"
    ]
  },
  {
    "name": "dondozo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/977.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "tatsugiri-curly",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/978.png",
    "types": [
      "dragon",
      "water"
    ]
  },
  {
    "name": "annihilape",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/979.png",
    "types": [
      "fighting",
      "ghost"
    ]
  },
  {
    "name": "clodsire",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/980.png",
    "types": [
      "poison",
      "ground"
    ]
  },
  {
    "name": "farigiraf",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/981.png",
    "types": [
      "normal",
      "psychic"
    ]
  },
  {
    "name": "dudunsparce-two-segment",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/982.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "kingambit",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/983.png",
    "types": [
      "dark",
      "steel"
    ]
  },
  {
    "name": "great-tusk",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/984.png",
    "types": [
      "ground",
      "fighting"
    ]
  },
  {
    "name": "scream-tail",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/985.png",
    "types": [
      "fairy",
      "psychic"
    ]
  },
  {
    "name": "brute-bonnet",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/986.png",
    "types": [
      "grass",
      "dark"
    ]
  },
  {
    "name": "flutter-mane",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/987.png",
    "types": [
      "ghost",
      "fairy"
    ]
  },
  {
    "name": "slither-wing",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/988.png",
    "types": [
      "bug",
      "fighting"
    ]
  },
  {
    "name": "sandy-shocks",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/989.png",
    "types": [
      "electric",
      "ground"
    ]
  },
  {
    "name": "iron-treads",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/990.png",
    "types": [
      "ground",
      "steel"
    ]
  },
  {
    "name": "iron-bundle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/991.png",
    "types": [
      "ice",
      "water"
    ]
  },
  {
    "name": "iron-hands",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/992.png",
    "types": [
      "fighting",
      "electric"
    ]
  },
  {
    "name": "iron-jugulis",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/993.png",
    "types": [
      "dark",
      "flying"
    ]
  },
  {
    "name": "iron-moth",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/994.png",
    "types": [
      "fire",
      "poison"
    ]
  },
  {
    "name": "iron-thorns",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/995.png",
    "types": [
      "rock",
      "electric"
    ]
  },
  {
    "name": "frigibax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/996.png",
    "types": [
      "dragon",
      "ice"
    ]
  },
  {
    "name": "arctibax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/997.png",
    "types": [
      "dragon",
      "ice"
    ]
  },
  {
    "name": "baxcalibur",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/998.png",
    "types": [
      "dragon",
      "ice"
    ]
  },
  {
    "name": "gimmighoul",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/999.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "gholdengo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1000.png",
    "types": [
      "steel",
      "ghost"
    ]
  },
  {
    "name": "wo-chien",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1001.png",
    "types": [
      "dark",
      "grass"
    ]
  },
  {
    "name": "chien-pao",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1002.png",
    "types": [
      "dark",
      "ice"
    ]
  },
  {
    "name": "ting-lu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1003.png",
    "types": [
      "dark",
      "ground"
    ]
  },
  {
    "name": "chi-yu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1004.png",
    "types": [
      "dark",
      "fire"
    ]
  },
  {
    "name": "roaring-moon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1005.png",
    "types": [
      "dragon",
      "dark"
    ]
  },
  {
    "name": "iron-valiant",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1006.png",
    "types": [
      "fairy",
      "fighting"
    ]
  },
  {
    "name": "koraidon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1007.png",
    "types": [
      "fighting",
      "dragon"
    ]
  },
  {
    "name": "miraidon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1008.png",
    "types": [
      "electric",
      "dragon"
    ]
  },
  {
    "name": "walking-wake",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1009.png",
    "types": [
      "water",
      "dragon"
    ]
  },
  {
    "name": "iron-leaves",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1010.png",
    "types": [
      "grass",
      "psychic"
    ]
  },
  {
    "name": "dipplin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1011.png",
    "types": [
      "grass",
      "dragon"
    ]
  },
  {
    "name": "poltchageist",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1012.png",
    "types": [
      "grass",
      "ghost"
    ]
  },
  {
    "name": "sinistcha",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1013.png",
    "types": [
      "grass",
      "ghost"
    ]
  },
  {
    "name": "okidogi",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1014.png",
    "types": [
      "poison",
      "fighting"
    ]
  },
  {
    "name": "munkidori",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1015.png",
    "types": [
      "poison",
      "psychic"
    ]
  },
  {
    "name": "fezandipiti",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1016.png",
    "types": [
      "poison",
      "fairy"
    ]
  },
  {
    "name": "ogerpon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1017.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "archaludon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1018.png",
    "types": [
      "steel",
      "dragon"
    ]
  },
  {
    "name": "hydrapple",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1019.png",
    "types": [
      "grass",
      "dragon"
    ]
  },
  {
    "name": "gouging-fire",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1020.png",
    "types": [
      "fire",
      "dragon"
    ]
  },
  {
    "name": "raging-bolt",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1021.png",
    "types": [
      "electric",
      "dragon"
    ]
  },
  {
    "name": "iron-boulder",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1022.png",
    "types": [
      "rock",
      "psychic"
    ]
  },
  {
    "name": "iron-crown",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1023.png",
    "types": [
      "steel",
      "psychic"
    ]
  },
  {
    "name": "terapagos",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1024.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "pecharunt",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/1025.png",
    "types": [
      "poison",
      "ghost"
    ]
  },
  {
    "name": "deoxys-attack",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10001.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "deoxys-defense",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10002.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "deoxys-speed",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10003.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "wormadam-sandy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10004.png",
    "types": [
      "bug",
      "ground"
    ]
  },
  {
    "name": "wormadam-trash",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10005.png",
    "types": [
      "bug",
      "steel"
    ]
  },
  {
    "name": "shaymin-sky",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10006.png",
    "types": [
      "grass",
      "flying"
    ]
  },
  {
    "name": "giratina-origin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10007.png",
    "types": [
      "ghost",
      "dragon"
    ]
  },
  {
    "name": "rotom-heat",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10008.png",
    "types": [
      "electric",
      "fire"
    ]
  },
  {
    "name": "rotom-wash",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10009.png",
    "types": [
      "electric",
      "water"
    ]
  },
  {
    "name": "rotom-frost",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10010.png",
    "types": [
      "electric",
      "ice"
    ]
  },
  {
    "name": "rotom-fan",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10011.png",
    "types": [
      "electric",
      "flying"
    ]
  },
  {
    "name": "rotom-mow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10012.png",
    "types": [
      "electric",
      "grass"
    ]
  },
  {
    "name": "castform-sunny",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10013.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "castform-rainy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10014.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "castform-snowy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10015.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "basculin-blue-striped",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10016.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "darmanitan-zen",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10017.png",
    "types": [
      "fire",
      "psychic"
    ]
  },
  {
    "name": "meloetta-pirouette",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10018.png",
    "types": [
      "normal",
      "fighting"
    ]
  },
  {
    "name": "tornadus-therian",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10019.png",
    "types": [
      "flying"
    ]
  },
  {
    "name": "thundurus-therian",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10020.png",
    "types": [
      "electric",
      "flying"
    ]
  },
  {
    "name": "landorus-therian",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10021.png",
    "types": [
      "ground",
      "flying"
    ]
  },
  {
    "name": "kyurem-black",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10022.png",
    "types": [
      "dragon",
      "ice"
    ]
  },
  {
    "name": "kyurem-white",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10023.png",
    "types": [
      "dragon",
      "ice"
    ]
  },
  {
    "name": "keldeo-resolute",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10024.png",
    "types": [
      "water",
      "fighting"
    ]
  },
  {
    "name": "meowstic-female",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10025.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "aegislash-blade",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10026.png",
    "types": [
      "steel",
      "ghost"
    ]
  },
  {
    "name": "pumpkaboo-small",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10027.png",
    "types": [
      "ghost",
      "grass"
    ]
  },
  {
    "name": "pumpkaboo-large",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10028.png",
    "types": [
      "ghost",
      "grass"
    ]
  },
  {
    "name": "pumpkaboo-super",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10029.png",
    "types": [
      "ghost",
      "grass"
    ]
  },
  {
    "name": "gourgeist-small",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10030.png",
    "types": [
      "ghost",
      "grass"
    ]
  },
  {
    "name": "gourgeist-large",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10031.png",
    "types": [
      "ghost",
      "grass"
    ]
  },
  {
    "name": "gourgeist-super",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10032.png",
    "types": [
      "ghost",
      "grass"
    ]
  },
  {
    "name": "venusaur-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10033.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "charizard-mega-x",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10034.png",
    "types": [
      "fire",
      "dragon"
    ]
  },
  {
    "name": "charizard-mega-y",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10035.png",
    "types": [
      "fire",
      "flying"
    ]
  },
  {
    "name": "blastoise-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10036.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "alakazam-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10037.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "gengar-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10038.png",
    "types": [
      "ghost",
      "poison"
    ]
  },
  {
    "name": "kangaskhan-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10039.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "pinsir-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10040.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "gyarados-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10041.png",
    "types": [
      "water",
      "dark"
    ]
  },
  {
    "name": "aerodactyl-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10042.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "mewtwo-mega-x",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10043.png",
    "types": [
      "psychic",
      "fighting"
    ]
  },
  {
    "name": "mewtwo-mega-y",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10044.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "ampharos-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10045.png",
    "types": [
      "electric",
      "dragon"
    ]
  },
  {
    "name": "scizor-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10046.png",
    "types": [
      "bug",
      "steel"
    ]
  },
  {
    "name": "heracross-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10047.png",
    "types": [
      "bug",
      "fighting"
    ]
  },
  {
    "name": "houndoom-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10048.png",
    "types": [
      "dark",
      "fire"
    ]
  },
  {
    "name": "tyranitar-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10049.png",
    "types": [
      "rock",
      "dark"
    ]
  },
  {
    "name": "blaziken-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10050.png",
    "types": [
      "fire",
      "fighting"
    ]
  },
  {
    "name": "gardevoir-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10051.png",
    "types": [
      "psychic",
      "fairy"
    ]
  },
  {
    "name": "mawile-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10052.png",
    "types": [
      "steel",
      "fairy"
    ]
  },
  {
    "name": "aggron-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10053.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "medicham-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10054.png",
    "types": [
      "fighting",
      "psychic"
    ]
  },
  {
    "name": "manectric-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10055.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "banette-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10056.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "absol-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10057.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "garchomp-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10058.png",
    "types": [
      "dragon",
      "ground"
    ]
  },
  {
    "name": "lucario-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10059.png",
    "types": [
      "fighting",
      "steel"
    ]
  },
  {
    "name": "abomasnow-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10060.png",
    "types": [
      "grass",
      "ice"
    ]
  },
  {
    "name": "floette-eternal",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10061.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "latias-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10062.png",
    "types": [
      "dragon",
      "psychic"
    ]
  },
  {
    "name": "latios-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10063.png",
    "types": [
      "dragon",
      "psychic"
    ]
  },
  {
    "name": "swampert-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10064.png",
    "types": [
      "water",
      "ground"
    ]
  },
  {
    "name": "sceptile-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10065.png",
    "types": [
      "grass",
      "dragon"
    ]
  },
  {
    "name": "sableye-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10066.png",
    "types": [
      "dark",
      "ghost"
    ]
  },
  {
    "name": "altaria-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10067.png",
    "types": [
      "dragon",
      "fairy"
    ]
  },
  {
    "name": "gallade-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10068.png",
    "types": [
      "psychic",
      "fighting"
    ]
  },
  {
    "name": "audino-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10069.png",
    "types": [
      "normal",
      "fairy"
    ]
  },
  {
    "name": "sharpedo-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10070.png",
    "types": [
      "water",
      "dark"
    ]
  },
  {
    "name": "slowbro-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10071.png",
    "types": [
      "water",
      "psychic"
    ]
  },
  {
    "name": "steelix-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10072.png",
    "types": [
      "steel",
      "ground"
    ]
  },
  {
    "name": "pidgeot-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10073.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "glalie-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10074.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "diancie-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10075.png",
    "types": [
      "rock",
      "fairy"
    ]
  },
  {
    "name": "metagross-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10076.png",
    "types": [
      "steel",
      "psychic"
    ]
  },
  {
    "name": "kyogre-primal",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10077.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "groudon-primal",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10078.png",
    "types": [
      "ground",
      "fire"
    ]
  },
  {
    "name": "rayquaza-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10079.png",
    "types": [
      "dragon",
      "flying"
    ]
  },
  {
    "name": "pikachu-rock-star",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10080.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "pikachu-belle",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10081.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "pikachu-pop-star",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10082.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "pikachu-phd",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10083.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "pikachu-libre",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10084.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "pikachu-cosplay",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10085.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "hoopa-unbound",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10086.png",
    "types": [
      "psychic",
      "dark"
    ]
  },
  {
    "name": "camerupt-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10087.png",
    "types": [
      "fire",
      "ground"
    ]
  },
  {
    "name": "lopunny-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10088.png",
    "types": [
      "normal",
      "fighting"
    ]
  },
  {
    "name": "salamence-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10089.png",
    "types": [
      "dragon",
      "flying"
    ]
  },
  {
    "name": "beedrill-mega",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10090.png",
    "types": [
      "bug",
      "poison"
    ]
  },
  {
    "name": "rattata-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10091.png",
    "types": [
      "dark",
      "normal"
    ]
  },
  {
    "name": "raticate-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10092.png",
    "types": [
      "dark",
      "normal"
    ]
  },
  {
    "name": "raticate-totem-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10093.png",
    "types": [
      "dark",
      "normal"
    ]
  },
  {
    "name": "pikachu-original-cap",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10094.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "pikachu-hoenn-cap",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10095.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "pikachu-sinnoh-cap",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10096.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "pikachu-unova-cap",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10097.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "pikachu-kalos-cap",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10098.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "pikachu-alola-cap",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10099.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "raichu-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10100.png",
    "types": [
      "electric",
      "psychic"
    ]
  },
  {
    "name": "sandshrew-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10101.png",
    "types": [
      "ice",
      "steel"
    ]
  },
  {
    "name": "sandslash-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10102.png",
    "types": [
      "ice",
      "steel"
    ]
  },
  {
    "name": "vulpix-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10103.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "ninetales-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10104.png",
    "types": [
      "ice",
      "fairy"
    ]
  },
  {
    "name": "diglett-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10105.png",
    "types": [
      "ground",
      "steel"
    ]
  },
  {
    "name": "dugtrio-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10106.png",
    "types": [
      "ground",
      "steel"
    ]
  },
  {
    "name": "meowth-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10107.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "persian-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10108.png",
    "types": [
      "dark"
    ]
  },
  {
    "name": "geodude-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10109.png",
    "types": [
      "rock",
      "electric"
    ]
  },
  {
    "name": "graveler-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10110.png",
    "types": [
      "rock",
      "electric"
    ]
  },
  {
    "name": "golem-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10111.png",
    "types": [
      "rock",
      "electric"
    ]
  },
  {
    "name": "grimer-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10112.png",
    "types": [
      "poison",
      "dark"
    ]
  },
  {
    "name": "muk-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10113.png",
    "types": [
      "poison",
      "dark"
    ]
  },
  {
    "name": "exeggutor-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10114.png",
    "types": [
      "grass",
      "dragon"
    ]
  },
  {
    "name": "marowak-alola",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10115.png",
    "types": [
      "fire",
      "ghost"
    ]
  },
  {
    "name": "greninja-battle-bond",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10116.png",
    "types": [
      "water",
      "dark"
    ]
  },
  {
    "name": "greninja-ash",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10117.png",
    "types": [
      "water",
      "dark"
    ]
  },
  {
    "name": "zygarde-10-power-construct",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10118.png",
    "types": [
      "dragon",
      "ground"
    ]
  },
  {
    "name": "zygarde-50-power-construct",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10119.png",
    "types": [
      "dragon",
      "ground"
    ]
  },
  {
    "name": "zygarde-complete",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10120.png",
    "types": [
      "dragon",
      "ground"
    ]
  },
  {
    "name": "gumshoos-totem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10121.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "vikavolt-totem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10122.png",
    "types": [
      "bug",
      "electric"
    ]
  },
  {
    "name": "oricorio-pom-pom",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10123.png",
    "types": [
      "electric",
      "flying"
    ]
  },
  {
    "name": "oricorio-pau",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10124.png",
    "types": [
      "psychic",
      "flying"
    ]
  },
  {
    "name": "oricorio-sensu",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10125.png",
    "types": [
      "ghost",
      "flying"
    ]
  },
  {
    "name": "lycanroc-midnight",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10126.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "wishiwashi-school",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10127.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "lurantis-totem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10128.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "salazzle-totem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10129.png",
    "types": [
      "poison",
      "fire"
    ]
  },
  {
    "name": "minior-orange-meteor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10130.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-yellow-meteor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10131.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-green-meteor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10132.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-blue-meteor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10133.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-indigo-meteor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10134.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-violet-meteor",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10135.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-red",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10136.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-orange",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10137.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-yellow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10138.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-green",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10139.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-blue",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10140.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-indigo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10141.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "minior-violet",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10142.png",
    "types": [
      "rock",
      "flying"
    ]
  },
  {
    "name": "mimikyu-busted",
    "artworkUrl": null,
    "types": [
      "ghost",
      "fairy"
    ]
  },
  {
    "name": "mimikyu-totem-disguised",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10144.png",
    "types": [
      "ghost",
      "fairy"
    ]
  },
  {
    "name": "mimikyu-totem-busted",
    "artworkUrl": null,
    "types": [
      "ghost",
      "fairy"
    ]
  },
  {
    "name": "kommo-o-totem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10146.png",
    "types": [
      "dragon",
      "fighting"
    ]
  },
  {
    "name": "magearna-original",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10147.png",
    "types": [
      "steel",
      "fairy"
    ]
  },
  {
    "name": "pikachu-partner-cap",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10148.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "marowak-totem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10149.png",
    "types": [
      "fire",
      "ghost"
    ]
  },
  {
    "name": "ribombee-totem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10150.png",
    "types": [
      "bug",
      "fairy"
    ]
  },
  {
    "name": "rockruff-own-tempo",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10151.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "lycanroc-dusk",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10152.png",
    "types": [
      "rock"
    ]
  },
  {
    "name": "araquanid-totem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10153.png",
    "types": [
      "water",
      "bug"
    ]
  },
  {
    "name": "togedemaru-totem",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10154.png",
    "types": [
      "electric",
      "steel"
    ]
  },
  {
    "name": "necrozma-dusk",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10155.png",
    "types": [
      "psychic",
      "steel"
    ]
  },
  {
    "name": "necrozma-dawn",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10156.png",
    "types": [
      "psychic",
      "ghost"
    ]
  },
  {
    "name": "necrozma-ultra",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10157.png",
    "types": [
      "psychic",
      "dragon"
    ]
  },
  {
    "name": "pikachu-starter",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10158.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "eevee-starter",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10159.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "pikachu-world-cap",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10160.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "meowth-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10161.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "ponyta-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10162.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "rapidash-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10163.png",
    "types": [
      "psychic",
      "fairy"
    ]
  },
  {
    "name": "slowpoke-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10164.png",
    "types": [
      "psychic"
    ]
  },
  {
    "name": "slowbro-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10165.png",
    "types": [
      "poison",
      "psychic"
    ]
  },
  {
    "name": "farfetchd-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10166.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "weezing-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10167.png",
    "types": [
      "poison",
      "fairy"
    ]
  },
  {
    "name": "mr-mime-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10168.png",
    "types": [
      "ice",
      "psychic"
    ]
  },
  {
    "name": "articuno-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10169.png",
    "types": [
      "psychic",
      "flying"
    ]
  },
  {
    "name": "zapdos-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10170.png",
    "types": [
      "fighting",
      "flying"
    ]
  },
  {
    "name": "moltres-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10171.png",
    "types": [
      "dark",
      "flying"
    ]
  },
  {
    "name": "slowking-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10172.png",
    "types": [
      "poison",
      "psychic"
    ]
  },
  {
    "name": "corsola-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10173.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "zigzagoon-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10174.png",
    "types": [
      "dark",
      "normal"
    ]
  },
  {
    "name": "linoone-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10175.png",
    "types": [
      "dark",
      "normal"
    ]
  },
  {
    "name": "darumaka-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10176.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "darmanitan-galar-standard",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10177.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "darmanitan-galar-zen",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10178.png",
    "types": [
      "ice",
      "fire"
    ]
  },
  {
    "name": "yamask-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10179.png",
    "types": [
      "ground",
      "ghost"
    ]
  },
  {
    "name": "stunfisk-galar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10180.png",
    "types": [
      "ground",
      "steel"
    ]
  },
  {
    "name": "zygarde-10",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10181.png",
    "types": [
      "dragon",
      "ground"
    ]
  },
  {
    "name": "cramorant-gulping",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10182.png",
    "types": [
      "flying",
      "water"
    ]
  },
  {
    "name": "cramorant-gorging",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10183.png",
    "types": [
      "flying",
      "water"
    ]
  },
  {
    "name": "toxtricity-low-key",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10184.png",
    "types": [
      "electric",
      "poison"
    ]
  },
  {
    "name": "eiscue-noice",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10185.png",
    "types": [
      "ice"
    ]
  },
  {
    "name": "indeedee-female",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10186.png",
    "types": [
      "psychic",
      "normal"
    ]
  },
  {
    "name": "morpeko-hangry",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10187.png",
    "types": [
      "electric",
      "dark"
    ]
  },
  {
    "name": "zacian-crowned",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10188.png",
    "types": [
      "fairy",
      "steel"
    ]
  },
  {
    "name": "zamazenta-crowned",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10189.png",
    "types": [
      "fighting",
      "steel"
    ]
  },
  {
    "name": "eternatus-eternamax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10190.png",
    "types": [
      "poison",
      "dragon"
    ]
  },
  {
    "name": "urshifu-rapid-strike",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10191.png",
    "types": [
      "fighting",
      "water"
    ]
  },
  {
    "name": "zarude-dada",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10192.png",
    "types": [
      "dark",
      "grass"
    ]
  },
  {
    "name": "calyrex-ice",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10193.png",
    "types": [
      "psychic",
      "ice"
    ]
  },
  {
    "name": "calyrex-shadow",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10194.png",
    "types": [
      "psychic",
      "ghost"
    ]
  },
  {
    "name": "venusaur-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10195.png",
    "types": [
      "grass",
      "poison"
    ]
  },
  {
    "name": "charizard-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10196.png",
    "types": [
      "fire",
      "flying"
    ]
  },
  {
    "name": "blastoise-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10197.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "butterfree-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10198.png",
    "types": [
      "bug",
      "flying"
    ]
  },
  {
    "name": "pikachu-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10199.png",
    "types": [
      "electric"
    ]
  },
  {
    "name": "meowth-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10200.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "machamp-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10201.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "gengar-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10202.png",
    "types": [
      "ghost",
      "poison"
    ]
  },
  {
    "name": "kingler-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10203.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "lapras-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10204.png",
    "types": [
      "water",
      "ice"
    ]
  },
  {
    "name": "eevee-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10205.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "snorlax-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10206.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "garbodor-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10207.png",
    "types": [
      "poison"
    ]
  },
  {
    "name": "melmetal-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10208.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "rillaboom-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10209.png",
    "types": [
      "grass"
    ]
  },
  {
    "name": "cinderace-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10210.png",
    "types": [
      "fire"
    ]
  },
  {
    "name": "inteleon-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10211.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "corviknight-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10212.png",
    "types": [
      "flying",
      "steel"
    ]
  },
  {
    "name": "orbeetle-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10213.png",
    "types": [
      "bug",
      "psychic"
    ]
  },
  {
    "name": "drednaw-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10214.png",
    "types": [
      "water",
      "rock"
    ]
  },
  {
    "name": "coalossal-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10215.png",
    "types": [
      "rock",
      "fire"
    ]
  },
  {
    "name": "flapple-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10216.png",
    "types": [
      "grass",
      "dragon"
    ]
  },
  {
    "name": "appletun-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10217.png",
    "types": [
      "grass",
      "dragon"
    ]
  },
  {
    "name": "sandaconda-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10218.png",
    "types": [
      "ground"
    ]
  },
  {
    "name": "toxtricity-amped-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10219.png",
    "types": [
      "electric",
      "poison"
    ]
  },
  {
    "name": "centiskorch-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10220.png",
    "types": [
      "fire",
      "bug"
    ]
  },
  {
    "name": "hatterene-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10221.png",
    "types": [
      "psychic",
      "fairy"
    ]
  },
  {
    "name": "grimmsnarl-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10222.png",
    "types": [
      "dark",
      "fairy"
    ]
  },
  {
    "name": "alcremie-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10223.png",
    "types": [
      "fairy"
    ]
  },
  {
    "name": "copperajah-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10224.png",
    "types": [
      "steel"
    ]
  },
  {
    "name": "duraludon-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10225.png",
    "types": [
      "steel",
      "dragon"
    ]
  },
  {
    "name": "urshifu-single-strike-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10226.png",
    "types": [
      "fighting",
      "dark"
    ]
  },
  {
    "name": "urshifu-rapid-strike-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10227.png",
    "types": [
      "fighting",
      "water"
    ]
  },
  {
    "name": "toxtricity-low-key-gmax",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10228.png",
    "types": [
      "electric",
      "poison"
    ]
  },
  {
    "name": "growlithe-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10229.png",
    "types": [
      "fire",
      "rock"
    ]
  },
  {
    "name": "arcanine-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10230.png",
    "types": [
      "fire",
      "rock"
    ]
  },
  {
    "name": "voltorb-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10231.png",
    "types": [
      "electric",
      "grass"
    ]
  },
  {
    "name": "electrode-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10232.png",
    "types": [
      "electric",
      "grass"
    ]
  },
  {
    "name": "typhlosion-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10233.png",
    "types": [
      "fire",
      "ghost"
    ]
  },
  {
    "name": "qwilfish-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10234.png",
    "types": [
      "dark",
      "poison"
    ]
  },
  {
    "name": "sneasel-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10235.png",
    "types": [
      "fighting",
      "poison"
    ]
  },
  {
    "name": "samurott-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10236.png",
    "types": [
      "water",
      "dark"
    ]
  },
  {
    "name": "lilligant-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10237.png",
    "types": [
      "grass",
      "fighting"
    ]
  },
  {
    "name": "zorua-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10238.png",
    "types": [
      "normal",
      "ghost"
    ]
  },
  {
    "name": "zoroark-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10239.png",
    "types": [
      "normal",
      "ghost"
    ]
  },
  {
    "name": "braviary-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10240.png",
    "types": [
      "psychic",
      "flying"
    ]
  },
  {
    "name": "sliggoo-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10241.png",
    "types": [
      "steel",
      "dragon"
    ]
  },
  {
    "name": "goodra-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10242.png",
    "types": [
      "steel",
      "dragon"
    ]
  },
  {
    "name": "avalugg-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10243.png",
    "types": [
      "ice",
      "rock"
    ]
  },
  {
    "name": "decidueye-hisui",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10244.png",
    "types": [
      "grass",
      "fighting"
    ]
  },
  {
    "name": "dialga-origin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10245.png",
    "types": [
      "steel",
      "dragon"
    ]
  },
  {
    "name": "palkia-origin",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10246.png",
    "types": [
      "water",
      "dragon"
    ]
  },
  {
    "name": "basculin-white-striped",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10247.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "basculegion-female",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10248.png",
    "types": [
      "water",
      "ghost"
    ]
  },
  {
    "name": "enamorus-therian",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10249.png",
    "types": [
      "fairy",
      "flying"
    ]
  },
  {
    "name": "tauros-paldea-combat-breed",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10250.png",
    "types": [
      "fighting"
    ]
  },
  {
    "name": "tauros-paldea-blaze-breed",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10251.png",
    "types": [
      "fighting",
      "fire"
    ]
  },
  {
    "name": "tauros-paldea-aqua-breed",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10252.png",
    "types": [
      "fighting",
      "water"
    ]
  },
  {
    "name": "wooper-paldea",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10253.png",
    "types": [
      "poison",
      "ground"
    ]
  },
  {
    "name": "oinkologne-female",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10254.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "dudunsparce-three-segment",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10255.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "palafin-hero",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10256.png",
    "types": [
      "water"
    ]
  },
  {
    "name": "maushold-family-of-three",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10257.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "tatsugiri-droopy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10258.png",
    "types": [
      "dragon",
      "water"
    ]
  },
  {
    "name": "tatsugiri-stretchy",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10259.png",
    "types": [
      "dragon",
      "water"
    ]
  },
  {
    "name": "squawkabilly-blue-plumage",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10260.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "squawkabilly-yellow-plumage",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10261.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "squawkabilly-white-plumage",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10262.png",
    "types": [
      "normal",
      "flying"
    ]
  },
  {
    "name": "gimmighoul-roaming",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10263.png",
    "types": [
      "ghost"
    ]
  },
  {
    "name": "koraidon-limited-build",
    "artworkUrl": null,
    "types": [
      "fighting",
      "dragon"
    ]
  },
  {
    "name": "koraidon-sprinting-build",
    "artworkUrl": null,
    "types": [
      "fighting",
      "dragon"
    ]
  },
  {
    "name": "koraidon-swimming-build",
    "artworkUrl": null,
    "types": [
      "fighting",
      "dragon"
    ]
  },
  {
    "name": "koraidon-gliding-build",
    "artworkUrl": null,
    "types": [
      "fighting",
      "dragon"
    ]
  },
  {
    "name": "miraidon-low-power-mode",
    "artworkUrl": null,
    "types": [
      "electric",
      "dragon"
    ]
  },
  {
    "name": "miraidon-drive-mode",
    "artworkUrl": null,
    "types": [
      "electric",
      "dragon"
    ]
  },
  {
    "name": "miraidon-aquatic-mode",
    "artworkUrl": null,
    "types": [
      "electric",
      "dragon"
    ]
  },
  {
    "name": "miraidon-glide-mode",
    "artworkUrl": null,
    "types": [
      "electric",
      "dragon"
    ]
  },
  {
    "name": "ursaluna-bloodmoon",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10272.png",
    "types": [
      "ground",
      "normal"
    ]
  },
  {
    "name": "ogerpon-wellspring-mask",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10273.png",
    "types": [
      "grass",
      "water"
    ]
  },
  {
    "name": "ogerpon-hearthflame-mask",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10274.png",
    "types": [
      "grass",
      "fire"
    ]
  },
  {
    "name": "ogerpon-cornerstone-mask",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10275.png",
    "types": [
      "grass",
      "rock"
    ]
  },
  {
    "name": "terapagos-terastal",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10276.png",
    "types": [
      "normal"
    ]
  },
  {
    "name": "terapagos-stellar",
    "artworkUrl": "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/other/official-artwork/10277.png",
    "types": [
      "normal"
    ]
  }
];

// src/pokemon.tsx
var import_jsx_runtime = require("react/jsx-runtime");
var typeToColor = {
  "grass": import_api.Color.Green,
  "water": import_api.Color.Blue,
  "fire": import_api.Color.Red,
  "ground": import_api.Color.Orange,
  "psy": import_api.Color.Magenta,
  "bug": import_api.Color.Green,
  "dragon": import_api.Color.Purple,
  "electric": import_api.Color.Yellow
};
var Command = () => {
  const [isLoading, setIsLoading] = (0, import_react.useState)(true);
  const [filteredPokemons, setFilteredPokemons] = (0, import_react.useState)(pokemons_default);
  const [activePokemon, setActivePokemon] = (0, import_react.useState)(null);
  const handleFilter = (s) => {
    const filtered = [];
    for (const pokemon of pokemons_default) {
      if (pokemon.artworkUrl && pokemon.name.includes(s)) filtered.push(pokemon);
    }
    console.log("results", filtered.length);
    setFilteredPokemons(filtered);
  };
  return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(
    import_api.Grid,
    {
      onSelectionChange: (id) => {
        setActivePokemon(id);
      },
      navigationTitle: `Select pokemon - ${activePokemon}`,
      searchBarPlaceholder: "Select pokemon",
      isLoading: false,
      onSearchTextChange: handleFilter,
      children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_api.Grid.Section, { title: "Pokemons", children: filteredPokemons.map((pokemon) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)(
        import_api.Grid.Item,
        {
          id: pokemon.name,
          title: pokemon.name,
          content: pokemon.artworkUrl
        },
        pokemon.name
      )) })
    }
  );
};
var pokemon_default = Command;
