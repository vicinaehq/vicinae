import { List,  Detail, Action, useApplications, Keyboard, Grid, ActionPanel, showToast } from '@omnicast/api';
import React, { useState } from 'react';
import pokemons from './pokemons';


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
		<Grid
			onSelectionChange={(id) => { setActivePokemon(id); }}
			navigationTitle={`Select pokemon - ${activePokemon}`}
			searchBarPlaceholder={'Select pokemon'}
			isLoading={false}
			onSearchTextChange={handleFilter}
		>
			<Grid.Section title={`Pokemons ${pokemons.length}`} columns={8}>
				{filteredPokemons.filter((p) => !!p.artworkUrl).map((pokemon) => (
					<Grid.Item 
						key={pokemon.name}
						id={pokemon.name}
						title={pokemon.name} 
						content={pokemon.artworkUrl!}
						actions={
							<ActionPanel>
								<Action title="file" onAction={() => { showToast({ title: `selected: ${pokemon.name}` }); }}>
								</Action>
							</ActionPanel>
						}
					/>
				))}
			</Grid.Section>
		</Grid>
	);
}

export default Command;
