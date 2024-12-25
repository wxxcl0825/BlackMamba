#import "@preview/algo:0.3.4": algo, i, d, comment, code
#import "@preview/tablex:0.0.8": tablex, colspanx, rowspanx
#import "@preview/codly:1.1.1": *
#import "@preview/codly-languages:0.1.1": *

#let thickness = 0.8pt
#let offset = 4pt
#let ubox(..) = box(
  width: 1fr,
  baseline: offset,
  stroke: (bottom: thickness),
)
#let uline(body) = {
  ubox()
  underline(
    stroke: thickness,
    offset: offset,
  )[#body]
  ubox()
}

#let fake_par = context {
  let b = par[#box()]
  let t = measure(b + b)
  b
  v(-t.height)
}

#let codeblock(filename) = {
  align(center)[
    #set text(size: 15pt)
    #filename.split("/").at(-1)
  ]
  set par(justify: false)
  raw(
    read(filename),
    block: true,
    lang: filename.split(".").at(-1)
  )
}

#let problem_counter = counter("problem")
#let problem(content) = {
  context {
    let cnt = problem_counter.get().first()
    if (cnt != 0) {
      v(2em)
    }
    set text(weight: "bold", fill: gray.darken(70%), font: "Source Han Sans")
    rect(width: 100%, inset: 10pt, fill: gray.lighten(50%))[
      #numbering("1. ", cnt + 1) #content
    ]
    problem_counter.step()
  }
  fake_par
}

#let chapter(content) = {
  let first_key = content.body.fields().keys().at(0)
  let header = content.body.at(first_key)
  if (header.split(":").at(0).at(0) == [Appendix]) {
    set text(size: 20pt)
    pagebreak()
    [Appendix]
    set text(size: 24pt)
    v(1cm)
    h(-1em); header.split(":").at(0).at(3)
    v(1.5cm)
    return;
  }

  let first_level = counter(heading).display("1")
  if (first_level != "0") {
    if (first_level != "1") {
      pagebreak()
    }
    set text(size: 22pt)
    [第#numbering("一", int(first_level))部分]
  }
  set text(size: 24pt)
  v(1cm)
  h(-2em); content.body
  v(1.5cm)
}

#let defconfig(
  has_chapter: true,
  language: "zh",
  outline_depth: 3,
  doc
) = {
  set list(marker: ([♢], [▹], [-]))
  set enum(numbering: "1.a.i.")
  set par(first-line-indent: 2em, leading: 11pt, justify: true)
  set text(font: "Source Han Serif SC") if language == "zh"

  // Set break
  show figure: it => it + fake_par
  show list: it => it + fake_par + v(1pt)
  show enum: it => it + fake_par + v(1pt)
  show grid: it => it + fake_par
  show heading: it => it + fake_par
  show math.equation.where(block: true): it => it + fake_par

  // Set heading
  set heading(numbering: "1.")
  show heading.where(level: 1): chapter
  show heading.where(level: 2): set text(size: 18pt)
  show heading.where(level: 3): set text(size: 15pt)
  show heading.where(level: 4): set text(size: 12pt, font: "LXGW WenKai")
  show heading: it => {
    let no = counter(heading).get().at(it.level - 1, default: 1)
    let above = 0pt
    let below = 0pt

    if (it.level == 1) {
      below = 8pt
    } else if (it.level == 2) {
      above = if no == 1 {10pt} else {30pt}
      below = 8pt
    } else if (it.level == 3) {
      above = if no == 1 {0pt} else {20pt}
      below = 4pt
    } else if (it.level == 4) {
      above = if no == 1 {2pt} else {10pt}
      below = 2pt
    } else {
      below = -2pt
    }

    if (it.level > 1) {
      v(above)
    }
    
    it
    v(below)
  }

  // Set link
  show link: set text(fill: eastern.darken(20%))

  // Set reference
  show ref: set text(fill: eastern.darken(20%))
  set heading(supplement: "章节")

  // Set figure
  show figure.where(kind: table): set text(font: "LXGW WenKai") if language == "zh"
  show figure.where(kind: table): set figure(supplement: "表", gap: 6pt) if language == "zh"
  show figure.where(kind: image): set text(font: "LXGW WenKai") if language == "zh"
  show figure.where(kind: image): set figure(supplement: "图", gap: 6pt) if language == "zh"
  show figure.caption: it => {
    if (language == "zh") {
      set text(size: 8pt)
      context [图#it.counter.display(it.numbering)：#it.body]
     } else {
      it
     }
  }

  set outline(depth: outline_depth)
  show outline.entry: it => {
    set text(size: 12pt)
    h((it.level - 1) * 1.3em)
    if (it.level == 1) {
      strong(it)
    } else if (it.level == 2) {
      it
    } else {
      it
    }
  }

  let icon(codepoint) = {
    box(
      height: 0.8em,
      baseline: 0.05em,
      image(codepoint)
    )
    h(0.1em)
  }

  show: codly-init
  // codly(languages: codly-languages)
  show raw: set text(font: ("Fira Code", "LXGW WenKai"))
  show raw.where(block: true): set par(justify: false)
  show raw.where(block: true): it => v(-5pt) + it + fake_par
  show raw.where(block: false): box.with(
    fill: luma(240),
    inset: (x: 3pt, y: 1pt),
    outset: (y: 3pt),
    radius: 2pt,
  )

  doc
}