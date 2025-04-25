import { List, Color,  Detail, Action, useApplications, Keyboard } from '@omnicast/api';
import React, { useState } from 'react';
import pokemons from './pokemons';

const typeToColor: Record<string, Color> = {
	'grass': Color.Green,
	'water': Color.Blue,
	'fire': Color.Red,
	'ground': Color.Orange,
	'psy': Color.Magenta,
	'bug': Color.Green,
	'dragon': Color.Purple,
	'electric': Color.Yellow,
};


const makeMarkdown = (pokemon: any) => {
	return `![${pokemon.name}](${pokemon.artworkUrl})`;
}

const PokemonDetail: React.FC<{ pokemon: any }> = ({ pokemon }) => {
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
}

const Command = () => {
	const [isLoading, setIsLoading] = useState(true);
	const [filteredPokemons, setFilteredPokemons] = useState(pokemons);
	const [activePokemon, setActivePokemon] = useState<string | null>(null);
	
	const handleFilter = (s: string) => {
		const filtered: any[] = [];

		for (const pokemon of pokemons) {
			if (pokemon.artworkUrl && pokemon.name.includes(s)) filtered.push(pokemon);
		}

		console.log('results', filtered.length);

		setFilteredPokemons(filtered);
	}


	return (
		<List 
			onSelectionChange={(id) => { setActivePokemon(id); }}
			navigationTitle={`Select pokemon - ${activePokemon}`}
			searchBarPlaceholder={'Select pokemon'}
			isLoading={false}
			onSearchTextChange={handleFilter}
			isShowingDetail
		>
			<List.Section title="Pokemons">
				{filteredPokemons.map((pokemon) => (
					<List.Item 
						detail={
							<List.Item.Detail markdown={makeMarkdown(pokemon)} />
						}
						key={pokemon.name}
						id={pokemon.name}
						title={pokemon.name} 
						icon={pokemon.artworkUrl!}
					/>
				))}
			</List.Section>
		</List>
	);
}

export default Command;
