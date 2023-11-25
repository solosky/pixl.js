import Vue from 'vue'
import ElementUI from 'element-ui'
import 'element-ui/lib/theme-chalk/index.css'
import App from './RealApp.vue' // './App.vue'

import i18n from './i18n' // internationalization
import router from './router'
import VueCookie from 'vue-cookie'

Vue.use(ElementUI, {
  i18n: (key, value) => i18n.t(key, value)
})

Vue.use(VueCookie)

new Vue({
  el: '#app',
  i18n,
  router,
  render: h => h(App)
})
