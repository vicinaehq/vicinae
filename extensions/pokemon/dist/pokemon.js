"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const jsx_runtime_1 = require("react/jsx-runtime");
const api_1 = require("@omnicast/api");
const react_1 = require("react");
const pokemons_1 = __importDefault(require("./pokemons"));
const typeToColor = {
    'grass': api_1.Color.Green,
    'water': api_1.Color.Blue,
    'fire': api_1.Color.Red,
    'ground': api_1.Color.Orange,
    'psy': api_1.Color.Magenta,
    'bug': api_1.Color.Green,
    'dragon': api_1.Color.Purple,
    'electric': api_1.Color.Yellow,
};
const makeMarkdown = (pokemon) => {
    return `![${pokemon.name}](${pokemon.artworkUrl})`;
};
const PokemonDetail = ({ pokemon }) => {
    const md = makeMarkdown(pokemon);
    return null;
    /*
    return (
        <Detail
            navigationTitle={`${pokemon.name} details`}
            markdown={md}
            metadata={
                <Detail.Metadata>
                        <Detail.Metadata.Label title="Weight" text={'50'} />
                        <Detail.Metadata.Label title="Base experience" text={'50'} />
                        <Detail.Metadata.Separator />
                        <Detail.Metadata.TagList title="Types">
                            {pokemon.types.map((color: any) => (
                                <List.Item.Detail.Metadata.TagList.Item key="color"
                                    color={typeToColor[color] ?? Color.PrimaryText}
                                text={color}
                            />
                            ))}
                        </Detail.Metadata.TagList>
                        <Detail.Metadata.Separator />
                        <Detail.Metadata.Label title="Other" text={'100'} />
                        <Detail.Metadata.Label title="Base pv" text={'50'} />
                        <Detail.Metadata.Label title="Original name" text={'ピカチュ'} />
                </Detail.Metadata>
            }
        />
    );
    */
};
const Command = () => {
    const [isLoading, setIsLoading] = (0, react_1.useState)(true);
    const [filteredPokemons, setFilteredPokemons] = (0, react_1.useState)([]);
    const [activePokemon, setActivePokemon] = (0, react_1.useState)(null);
    const handleFilter = (s) => {
        const filtered = [];
        for (const pokemon of pokemons_1.default) {
            if (pokemon.artworkUrl && pokemon.name.includes(s))
                filtered.push(pokemon);
        }
        console.log('results', filtered.length);
        setFilteredPokemons(filtered);
    };
    return ((0, jsx_runtime_1.jsx)(api_1.List, { onSelectionChange: (id) => { setActivePokemon(id); }, navigationTitle: `Select pokemon - ${activePokemon}`, searchBarPlaceholder: 'Select pokemon', isLoading: false, onSearchTextChange: handleFilter, children: (0, jsx_runtime_1.jsx)(api_1.List.Section, { title: "Pokemons", children: filteredPokemons.map((pokemon) => ((0, jsx_runtime_1.jsx)(api_1.List.Item, { detail: makeMarkdown(pokemon), id: pokemon.name, title: pokemon.name, icon: pokemon.artworkUrl }, pokemon.name))) }) }));
};
exports.default = Command;
