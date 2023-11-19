import Home from "~/App.vue" // './Home.vue'

const routes = [
    {path: '*', component: Home}, // TODO 404 page
    {path: '/', component: Home},
    {
        path: '/a/:game_id-:amiibo_id', beforeEnter: (to, _from, _next) => {
            // https://pixl.amiibo.xyz/a/01010000-000E0002
            // TODO 临时重定向到amiibo.life
            window.location.replace(`https://amiibo.life/nfc/${to.params.game_id.toUpperCase()}-${to.params.amiibo_id.toUpperCase()}`)
        }
    }
]

export default routes
