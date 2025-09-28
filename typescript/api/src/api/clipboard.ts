import { PathLike } from "fs";
import { bus } from "./bus";
import { ClipboardContent } from "./proto/clipboard";

export namespace Clipboard {
  export type Content =
    | { text: string }
    | { file: PathLike }
    | { html: string; text?: string };
  export type ReadContent =
    | { text: string }
    | { file?: string }
    | { html?: string };
  export type CopyOptions = {
    concealed?: boolean;
  };

  function mapContent(content: string | number | Clipboard.Content): ClipboardContent {
    let ct = ClipboardContent.create();

    if (typeof content != "object") {
      ct.text = `${content}`;
    } else {
      if (content["file"]) {
        ct.path = { path: content["file"] };
      } else if (content["html"]) {
        ct.html = { html: content["html"], text: content["text"] };
      } else {
        ct.text = content["text"];
      }
    }

    return ct;
  }

  export async function copy(
    text: string | number | Clipboard.Content,
    options: Clipboard.CopyOptions = {},
  ) {
    await bus.turboRequest("clipboard.copy", {
      content: mapContent(text),
      options: { concealed: options.concealed ?? false },
    });
  }

  export async function paste(text: string | Clipboard.Content) {
    await bus.turboRequest("clipboard.paste", {
      content: mapContent(text),
    });
  }

  export async function read(options?: { offset?: number }): Promise<Clipboard.ReadContent> {
	throw new Error('Clipboard.read not implemented');
  }

  export async function readText(options?: { offset?: number }): Promise<string | undefined> {
	throw new Error('Clipboard.readText not implemented');
  }

  export async function clear(text: string) {
	throw new Error('Clibpoard.clear not implemented');
  }
}
