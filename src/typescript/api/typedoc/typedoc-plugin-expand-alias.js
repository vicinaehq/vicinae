import fs from "node:fs"
import { Comment, Converter, ReferenceType, ReflectionKind } from "typedoc"
import ts from "typescript"

export function load(app) {
  app.converter.on(Converter.EVENT_RESOLVE_END, (context) => {
    const project = context.project

    const allAliases = project.getReflectionsByKind(ReflectionKind.TypeAlias)

    for (const typeAlias of allAliases) {
      if (shouldSkip(typeAlias)) continue

      const enumInfo = resolveEnumForAlias(typeAlias, project)

      if (!enumInfo) continue

      injectEnumDocumentation(typeAlias, enumInfo)
    }
  })
}

function shouldSkip(typeAlias) {
  const comment = typeAlias.comment
  if (!comment) return false

  const noExpandTag =
    comment.getTag?.("@noExpand") || comment.modifierTags?.has("@noExpand")

  return Boolean(noExpandTag)
}

function resolveEnumForAlias(typeAlias, project) {
  const aliasType = typeAlias.type
  if (!(aliasType instanceof ReferenceType)) return null

  // Attempt reflection resolve
  const reflectionEnum = findEnumReflectionByName(project, aliasType.name)
  if (reflectionEnum) {
    return buildEnumInfoFromReflection(reflectionEnum)
  }

  // Attempt AST resolve
  const sourceFilePath = aliasType._target?.fileName
  if (!sourceFilePath || !fs.existsSync(sourceFilePath)) {
    return null
  }

  return buildEnumInfoFromAST(sourceFilePath, aliasType.name)
}

function findEnumReflectionByName(project, name) {
  function search(reflection) {
    if (!reflection) return null

    if (reflection.kind === ReflectionKind.Enum && reflection.name === name)
      return reflection

    if (reflection.children) {
      for (const child of reflection.children) {
        const result = search(child)
        if (result) return result
      }
    }

    return null
  }

  return search(project)
}

function buildEnumInfoFromReflection(enumReflection) {
  return {
    name: enumReflection.name,
    comment: enumReflection.comment,
    members: (enumReflection.children || []).map((child) => {
      const value =
        child.type?.value ??
        child.defaultValue?.replace(/['"]/g, "") ??
        child.name.toLowerCase()

      return {
        name: child.name,
        value,
        comment: (child.comment?.summary || [])
          .map((p) => p.text || "")
          .join(""),
        parts: child.comment?.summary || [],
      }
    }),
  }
}

function buildEnumInfoFromAST(sourceFilePath, enumName) {
  const sourceText = fs.readFileSync(sourceFilePath, "utf-8")
  const tsSource = ts.createSourceFile(
    sourceFilePath,
    sourceText,
    ts.ScriptTarget.Latest,
    true,
  )

  let foundEnum = null
  ts.forEachChild(tsSource, function visit(node) {
    if (ts.isEnumDeclaration(node) && node.name.text === enumName) {
      foundEnum = node
    }
    ts.forEachChild(node, visit)
  })

  if (!foundEnum) return null

  const members = foundEnum.members.map((member) => {
    const name = member.name.getText(tsSource)
    const initializer = member.initializer?.getText(tsSource)
    const value = initializer
      ? initializer.replace(/['"]/g, "")
      : name.toLowerCase()

    const jsDoc = ts.getJSDocCommentsAndTags(member) || []
    const comment = jsDoc
      .map((doc) => doc.getText(tsSource))
      .join(" ")
      .replace(/\/\*\*|\*\//g, "")
      .replace(/^\s*\*\s?/gm, "")
      .trim()

    return {
      name,
      value,
      comment,
      parts: [{ kind: "text", text: comment }],
    }
  })

  return { name: enumName, comment: null, members }
}

function injectEnumDocumentation(typeAlias, enumInfo) {
  if (!typeAlias.comment) typeAlias.comment = new Comment([], [])
  const summary = typeAlias.comment.summary

  // Add main enum comment
  if (enumInfo.comment?.summary) {
    const mainText = enumInfo.comment.summary
      .map((p) => p.text || "")
      .join(" ")
    if (mainText) summary.push({ kind: "text", text: `${mainText}\n\n` })
  }

  summary.push({ kind: "text", text: "## Enumeration Members\n\n" })

  for (const member of enumInfo.members) {
    const displayValue =
      typeof member.value === "string"
        ? `"${member.value}"`
        : String(member.value)

    summary.push({ kind: "text", text: `### ${member.name}\n\n` })
    summary.push({
      kind: "text",
      text: `> **${member.name}**: \`${displayValue}\`\n\n`,
    })

    if (member.parts) summary.push(...member.parts)
    summary.push({ kind: "text", text: "\n\n---\n" })
  }
}
