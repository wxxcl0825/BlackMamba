#import "template.typ": *

#let cover(title, subtitle, date) = {
  set align(center)
  v(36pt)
  image("images/ZJU-logo.svg", width: 60%)
  v(36pt)

  if (subtitle == none) {
    v(15pt)
  }

  set text(size: 32pt, weight: "bold", font: "Source Han Serif SC")
  title

  if (subtitle != none) {
    v(-4pt)
    set text(size: 22pt, weight: "regular", font: "Source Han Sans SC")
    subtitle
  } else {
    v(20pt)
  }
  v(60pt)

  set text(size: 15pt, weight: "regular", font: ("Libertinus Serif", "LXGW WenKai"))
  grid(
    columns: (80pt, 150pt),
    row-gutter: 16pt,
    align: horizon,
    [成员：], [#uline[薛辰立~~~~3220102215]],
    [], [#uline[吴海槟~~~~3220106042]],
    [], [#uline[李政达~~~~3220102159]],
    [], [],
    [日期：], [#uline[#datetime.today().display("[year]年[month padding:none]月[day padding:none]日")]],
  )
  pagebreak()
}

#let report(
  title: none,
  subtitle: none,
  date: none,
  outline_depth: 3,
  doc
) = {
  show: defconfig.with(outline_depth: outline_depth)

  set heading(numbering: (..args) => {
    let nums = args.pos()
    if (nums.len() == 1) {
      if (nums.at(0) == 7) {
        return []
      }
      return [Part ] + numbering("I", nums.at(0))
    } else if (nums.len() == 2) {
      return numbering("一、", nums.at(1)) + h(-5pt)
    } else if (nums.len() == 3) {
      return h(-10pt) + numbering("（一）", nums.at(2)) + h(-5pt)
    }
  })

  {
    set par(first-line-indent: 0em)
    cover(title, subtitle, date)
    outline(title: "    目录")
  }

  set page(numbering: "1 / 1")
  counter(page).update(1)

  doc
}

#show: doc => report(
  title: "Project: Black Mamba",
  subtitle: "计算机图形学课程大项目",
  doc)

#codly(zebra-fill: none)

#include "chapters/introduction.typ"

#include "chapters/architecture.typ"

#include "chapters/render.typ"

#include "chapters/physics.typ"

#include "chapters/audio.typ"

#include "chapters/game.typ"

#include "chapters/appendix.typ"

#bibliography("references.bib")