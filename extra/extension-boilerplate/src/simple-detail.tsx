import { Action, ActionPanel, Detail, showToast } from "@vicinae/api";
import {
	DATA_URI_PNG_BASE64,
	DATA_URI_SVG_BASE64,
	DATA_URI_SVG_PLAIN,
	DATA_URI_SVG_URLENCODED,
} from "./constants";

const intro = `# Hello world

Your extension is *working* successfully.

Now you can start making changes to this command source file and see it update live here.

If you are online, you should be able to see the Vicinae logo below:

![](https://github.com/vicinaehq/vicinae/raw/main/.github/assets/vicinae-banner.png?raycast-height=100)

---

# Markdown formatting
`;

const headings = `
# Heading 1
## Heading 2
### Heading 3
#### Heading 4
##### Heading 5
###### Heading 6

## heading with _italic_ and \`code\` in the middle
`;

const textDecorations = `
This text has **bold**, _italic_, ***bold-italic***, and \`code\`.
`;

const lists = `
- List item 1
- List item 2
- List item 3

1. Ordered list item 1
2. Ordered list item 2
3. Ordered list item 3
`;

const images = `
blue square (data:image/svg+xml;utf8 plain):
<img width="100" height='100' src="${DATA_URI_SVG_PLAIN}">

green rectangle (data:image/svg+xml urlencoded):
<img width="200" height='50' src="${DATA_URI_SVG_URLENCODED}">

yellow rectangle with text (data:image/svg+xml base64):
<img width="200" height='100' src="${DATA_URI_SVG_BASE64}">

markdown format image (data:image/png;base64 with raycast-* size):

![Logo](${DATA_URI_PNG_BASE64}?raycast-width=50&raycast-height=50)

markdown format image with style attribute (no unit):

<img src=${DATA_URI_PNG_BASE64} style="width:50; height:50">

markdown format image with style attribute (px):

<img src=${DATA_URI_PNG_BASE64} style="width:50px; height:50px">
`;

const table = `
Table with formatting:

| Name         | Age | Country    |
|--------------|-----|------------|
| Alice        |  28 | **Canada** |
| Bob          |  34 | _Germany_  |
| Charlie      |  25 | \`Brazil\` |


Table with alignment:

| Left Align | Center Align | Right Align |
|:-----------|:------------:|------------:|
| Apple      |   Orange     |        Pear |
| Banana     |   Mango      |      Grape  |
`;

const content = [intro, headings, textDecorations, lists, images, table];

export default function SimpleDetail() {
	return (
		<Detail
			markdown={content.map((c) => c.trim()).join("\n\n")}
			actions={
				<ActionPanel>
					<Action
						title="Say hello"
						onAction={() => showToast({ title: "Hello!" })}
					/>
				</ActionPanel>
			}
		/>
	);
}
