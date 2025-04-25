import { writeFileSync } from "fs";

type Pokemon = {
	name: string;
	artworkUrl: string;
	types: string[];
};

const main = async () => {
	const res = await fetch('https://pokeapi.co/api/v2/pokemon?limit=1302');
	const { results } = await res.json();

	const pokemons: Pokemon[] = [];

	for (const { name, url } of results) {
		const res = await fetch(url);
		const pokemonData = await res.json();

		console.log('pushed data for ' + name);

		pokemons.push({
			name,
			artworkUrl: pokemonData.sprites.other['official-artwork'].front_default,
			types: pokemonData.types.map(t => t.type.name)
		});
	}

	writeFileSync('pokemons.json', JSON.stringify(pokemons, null, 2))
}

main();
